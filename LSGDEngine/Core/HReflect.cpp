#include "HCorePCH.h"
#include "HReflect.h"

// type database
#include "HTypeDatabaseUtils.h"

// reflection context
#include "HReflectionContext.h"

using namespace lsgd;
using namespace lsgd::reflect;

void HProperty::SerializeItem(HReflectionContext& InContext, uint8* OutData)
{
	int32 CurrOffset = Offset;
	for (int32 Index = 0; Index < ArrayDim; ++Index)
	{
		InContext.Serialize(OutData + CurrOffset, ElementSize);
		CurrOffset += ElementSize;
	}	
}

void HStruct::AddProperty(unique_ptr<HProperty>& InProperty)
{
	Properties.push_back(HMove(InProperty));
}

void HStruct::SerializeProperties(HReflectionContext& InContext, uint8* OutData) const
{
	for (auto& Property : Properties)
	{
		Property->SerializeItem(InContext, OutData);
	}
}

void HClass::AddMethod(unique_ptr<HFunction>& InMethod)
{
	HFunction* RawMethod = InMethod.get();

	// add method
	Methods.push_back(HMove(InMethod));

	// add pointer to mapper
	FunctionMap.insert({ RawMethod->Name, RawMethod });
}

uint32 HClass::GetTotalSize() const
{
	uint32 TotalSize = 0;

	// accumulate all properties which the HClass have
	for (const unique_ptr<HProperty>& Property : Properties)
	{
		TotalSize += Property->TotalSize;
	}

	// @todo: accumulate its base class's properties
	// ...

	return TotalSize;
}

HNativeFunction::HNativeFunction(unique_ptr<HNativeFunctionObject>& InNativeFunctionObject, const HStruct* InOwner)
	: HFunction(InNativeFunctionObject->FunctionName, InOwner)
{
	SetNativeFunctionObject(InNativeFunctionObject);
}

void HNativeFunction::CallFunction(void* InContext, const HFrame& InStack, void* const OutReturn)
{
	//@todo - need to implement
}

void HNativeFunction::InvokeInner(HDirectFunctionCallFrame& InvokeFrame)
{
	// generate input parameter property lists
	HArray<HProperty*> ParameterProperties;
	HProperty* ParameterProperty = FunctionInputHead;
	while (ParameterProperty != nullptr)
	{
		ParameterProperties.push_back(ParameterProperty);
		ParameterProperty = ParameterProperty->FunctionInputNext;
	}

	HNativeFunctionFrame NativeFrame;
	NativeFrame.SetFrame(InvokeFrame, ParameterProperties);

	// call the function
	NativeFunctionObject->CallFunction(NativeFrame);

	// add output to InvokeFrame
	int16 OutputSize;
	uint8 OutputBuffer[256];
	NativeFrame.GetOutput(OutputBuffer, OutputSize);

	InvokeFrame.PushOutput(OutputBuffer, OutputSize);
}

void HNativeFunction::SetNativeFunctionObject(unique_ptr<HNativeFunctionObject>& InNativeFunctionObject)
{
	// move the function object
	NativeFunctionObject = HMove(InNativeFunctionObject);
	
	// processing function inputs
	int8 AccParamNum = 0;
	int16 AccParamSize = 0;
	
	HProperty*& CurrFunctionInput = FunctionInputHead;
	for (int32 ParamIndex = 0; ParamIndex < (int32)NativeFunctionObject->FunctionInputs.size(); ++ParamIndex)
	{
		HTypeDescriptor& TypeDescriptor = NativeFunctionObject->FunctionInputs[ParamIndex].TypeDescriptor;		

		// create parameter property		
		HString ParamName = HString("Param") + ToString(ParamIndex);
		unique_ptr<HProperty> NewProperty = TypeDescriptor.CreateProperty(ParamName, AccParamSize);
		
		// link to function input link
		HProperty* NewPropertyRaw = NewProperty.get();
		NewPropertyRaw->FunctionInputNext = CurrFunctionInput;
		CurrFunctionInput = NewPropertyRaw;

		// add property to UStruct
		AddProperty(NewProperty);

		// last update accumulators
		AccParamNum++;
		AccParamSize += TypeDescriptor.GetSize();
	}

	ParamNum = AccParamNum;
	ParamSize = AccParamSize;

	// return value offset is next to parameters
	ReturnValueOffset = ParamSize;

	// processing function outputs
	unique_ptr<HProperty> FunctionOutput = NativeFunctionObject->FunctionOutput.TypeDescriptor.CreateProperty("ReturnParam", ReturnValueOffset);
	FunctionOutputHead = FunctionOutput.get();

	// add property to UStruct
	AddProperty(FunctionOutput);
}
