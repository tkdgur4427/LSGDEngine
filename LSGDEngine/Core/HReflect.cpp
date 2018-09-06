#include "HCorePCH.h"
#include "HReflect.h"

// type database
#include "HTypeDatabaseUtils.h"

using namespace lsgd;
using namespace lsgd::reflect;

void HStruct::AddProperty(unique_ptr<HProperty>& InProperty)
{
	Properties.push_back(move(InProperty));
}

HNativeFunction::HNativeFunction(unique_ptr<HNativeFunctionObject>& InNativeFunctionObject)
	: HFunction(InNativeFunctionObject->FunctionName)
{
	SetNativeFunctionObject(InNativeFunctionObject);
}

void HNativeFunction::CallFunction(void* InContext, const HFrame& InStack, void* const OutReturn)
{
	// call function with Stack~
	// @todo
}

void HNativeFunction::SetNativeFunctionObject(unique_ptr<HNativeFunctionObject>& InNativeFunctionObject)
{
	// move the function object
	NativeFunctionObject = move(InNativeFunctionObject);
	
	// processing function inputs
	int8 AccParamNum = 0;
	int16 AccParamSize = 0;
	
	HProperty*& CurrFunctionInput = FunctionInputHead;
	for (int32 ParamIndex = 0; ParamIndex < (int32)NativeFunctionObject->FunctionInputs.size(); ++ParamIndex)
	{
		HTypeDescriptor& TypeDescriptor = NativeFunctionObject->FunctionInputs[ParamIndex].TypeDescriptor;

		AccParamNum++;
		AccParamSize += TypeDescriptor.GetSize();

		// create parameter property		
		HString ParamName = HString("Param") + ToString(ParamIndex);
		unique_ptr<HProperty> NewProperty = TypeDescriptor.CreateProperty(ParamName, AccParamSize);
		
		// link to function input link
		HProperty* NewPropertyRaw = NewProperty.get();
		NewPropertyRaw->FunctionInputNext = CurrFunctionInput;
		CurrFunctionInput = NewPropertyRaw;

		// add property to UStruct
		AddProperty(NewProperty);
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
