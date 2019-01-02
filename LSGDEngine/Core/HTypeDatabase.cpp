#include "HCorePCH.h"
#include "HTypeDatabase.h"

// reflect
#include "HReflect.h"

//@todo... so many dependencies exists!
#include "HTypeDatabaseUtils.h"

using namespace lsgd;
using namespace lsgd::reflect;

int32 HTypeDescriptor::GetSize() const
{
	if (PrimitiveType != nullptr)
	{
		return PrimitiveType->Size;
	}
	if (ClassType != nullptr)
	{
		//... undefined yet	
	}

	return -1;
}

unique_ptr<HProperty> HTypeDescriptor::CreateProperty(const HString& InVariableName, int32 InOffset) const
{
	if (PrimitiveType != nullptr)
	{		
		return HTypeDatabase::GetSingleton()->CreatePrimitiveProperty(PrimitiveType->PrimitiveName, InVariableName, InOffset, PrimitiveType->Size);
	}
	else if (ClassType != nullptr)
	{
		// @todo
	}

	return nullptr;	
}

HTypeDatabase* HTypeDatabase::GetSingleton()
{
	static HTypeDatabase TypeDatabase;
	return &TypeDatabase;
}

bool HTypeDatabase::ExistPrimitiveType(const HString& InName) const
{
	for (auto& PrimitiveType : PrimitiveTypes)
	{
		if (PrimitiveType->PrimitiveName == InName)
		{
			return true;
		}
	}
	return false;
}

HString HTypeDatabase::FindPrimitiveTypeName(const HGuid& InGuid) const
{
	auto Iterator = GuidToPrimitiveTypes.find(InGuid);
	if (Iterator == GuidToPrimitiveTypes.end())
	{
		return HString();
	}
	
	return PrimitiveTypes[Iterator->second]->PrimitiveName;
}

const HPrimitiveType* HTypeDatabase::GetPrimitiveType(const HString& InName) const
{
	return PrimitiveTypes[NameToPrimitiveTypes.find(InName)->second].get();
}

void HTypeDatabase::AddClassTypeInner(const HString& InName, const HString& InSuperClassName)
{
	check(!ExistClass(InName));

	// declare the class instance
	unique_ptr<HClass> NewClass = lsgd::make_unique<HClass>(InName);
	Classes.push_back(HMove(NewClass));

	// only supoer class name exists
	if (InSuperClassName != HString())
	{
		// generate class data for post-processing
		HPostProcessClassData NewPostprocessClassData;
		NewPostprocessClassData.ClassName = InName;
		NewPostprocessClassData.SuperClassName = InSuperClassName;

		PostProcessClassDataList.push_back(NewPostprocessClassData);
	}
}

void HTypeDatabase::ProcessPostProcessClassData()
{
	for (auto& PostProcessClassData : PostProcessClassDataList)
	{
		// link the class with base class
		const HString& ClassName = PostProcessClassData.ClassName;
		const HString& BaseClassName = PostProcessClassData.SuperClassName;
		check(ExistClass(ClassName));
		check(ExistClass(BaseClassName));

		HClass* Class = GetClass(ClassName);
		HClass* BaseClass = GetClass(BaseClassName);

		Class->SuperStruct = BaseClass;
	}
}

bool HTypeDatabase::ExistClass(const HString& InClassName) const
{
	for (auto& Class : Classes)
	{
		if (Class->Name == InClassName)
		{
			return true;
		}
	}
	return false;
}

int32 HTypeDatabase::GetClassIndex(const HString& InClassName) const
{
	int32 Index = 0;
	for (auto& Class : Classes)
	{
		if (Class->Name == InClassName)
		{
			return Index;
		}

		Index++;
	}
	return -1;
}

const HClass* HTypeDatabase::GetClass(const HString& InClassName) const
{
	return Classes[GetClassIndex(InClassName)].get();
}

HClass* HTypeDatabase::GetClass(const HString& InClassName)
{
	return Classes[GetClassIndex(InClassName)].get();
}

const HCommonTypeHelperInterface* HTypeDatabase::GetClassCommonTypeHelper(const HString& InClassName)
{
	return ClassCommonTypeHelpers[GetClassIndex(InClassName)];
}

void HTypeDatabase::LinkProperty(int32 InClassIndex, unique_ptr<HProperty>& InProperty)
{
	Classes[InClassIndex]->AddProperty(InProperty);
}

void HTypeDatabase::LinkMethod(int32 InClassIndex, unique_ptr<HNativeFunctionObject>& InNativeFunctionObject)
{
	HClass* OwnerClass = Classes[InClassIndex].get();
	unique_ptr<HFunction> NewFunction = make_unique<HFunction, HNativeFunction>(InNativeFunctionObject, OwnerClass);
	OwnerClass->AddMethod(NewFunction);
}

unique_ptr<HProperty> HTypeDatabase::CreatePrimitiveProperty(const HString& InTypeName, const HString& InVariableName, int32 InOffset, int32 InSize, int32 InArrayDim) const
{
	unique_ptr<HProperty> NewProperty;

	unique_ptr<HTypeDescriptor> NewDescriptor = make_unique<HTypeDescriptor>(HTypeDatabaseUtils::GetTypeDescriptor(InTypeName));
	check(NewDescriptor->PrimitiveType != nullptr);

	const HPrimitiveType* FoundType = NewDescriptor->PrimitiveType;

	if (FoundType->IsNumber())
	{
		NewProperty = make_unique<HProperty, HNumberProperty>(InTypeName, InVariableName, InOffset, InSize, InArrayDim);
	}
	else if (FoundType->IsBoolean())
	{
		NewProperty = make_unique<HProperty, HBoolProperty>(InVariableName, InOffset, InSize, InArrayDim);
	}
	else if (FoundType->IsString())
	{
		NewProperty = make_unique<HProperty, HStringProperty>(InVariableName, InOffset, InSize, InArrayDim);
	}

	// move the type descriptor instance (note that unique_ptr)
	NewProperty->TypeDescriptor = HMove(NewDescriptor);

	return NewProperty;
}

unique_ptr<HProperty> HTypeDatabase::CreateClassProperty(const HString& InClassTypeName, const HString& InVariableName, int32 InOffset, int32 InArrayDim) const
{
	unique_ptr<HTypeDescriptor> NewDescriptor = make_unique<HTypeDescriptor>(HTypeDatabaseUtils::GetTypeDescriptor(InClassTypeName));
	check(NewDescriptor->ClassType != nullptr);

	unique_ptr<HProperty> NewProperty = make_unique<HProperty, HClassProperty>(InVariableName, InOffset, NewDescriptor->ClassType, InArrayDim);

	// move the type descriptor instance (note that unique_ptr)
	NewProperty->TypeDescriptor = HMove(NewDescriptor);

	return NewProperty;
}

unique_ptr<HProperty> HTypeDatabase::CreatePropertyByName(const HString& InTypeName, const HString& InVariableName, int32 InOffset, int32 InSize, int32 InArrayDim) const
{
	// first handling primitive type
	if (ExistPrimitiveType(InTypeName))
	{
		return HMove(CreatePrimitiveProperty(InTypeName, InVariableName, InOffset, InSize, InArrayDim));
	}

	// second ...		
	return nullptr;
}

void HNativeFunctionObject::ProcessDecomposedData(const HFunctionDecomposeResult& InData)
{
	IsClassFunction = InData.IsClassFunction;
	IsConst = InData.IsConst;

	if (IsClassFunction)
	{
		// setting owner class for native function
		Class = InData.ClassType;
	}

	FunctionOutput.TypeDescriptor = InData.OutputType;
	for (const HTypeDescriptor& Descriptor : InData.InputTypes)
	{
		FunctionInputs.push_back({ Descriptor });
	}
}

HString HNativeFunctionObject::GetClassName() const
{ 
	if (Class.ClassType != nullptr) 
	{ 
		return Class.ClassType->Name; 
	} 
	return HString(); 
}

HNativeFunctionFrame::HNativeFunctionFrame()
	: HVariadicStack()
	, ClassRefOffset(0)
	, ClassRefSize(0)
	, OutputOffset(0)
	, OutputSize(0)
{
}

void HNativeFunctionFrame::PushParameter(uint8* InData, int16 InDataSize)
{
	int16 Offset = GetTopOffset();

	// push parameter
	Push(InData, InDataSize);

	ParamOffsets.push_back(Offset);
	ParamSizes.push_back(InDataSize);
}

void HNativeFunctionFrame::SetClass(uint8* InClassReference)
{
	int16 Offset = GetTopOffset();

	// add class reference
	PushReference(InClassReference);

	ClassRefOffset = Offset;
	ClassRefSize = GetTopOffset() - Offset;
}

void HNativeFunctionFrame::SetFrame(HDirectFunctionCallFrame& InFrame, HArray<HProperty*> InParameters)
{
	// pop the value in reverse order
	uint8 DataBuffer[256];
	int16 CurrOffset = 0;

	for (int16 Index = (int16)(InParameters.size() - 1); Index >= 0; --Index)
	{
		HProperty* Property = InParameters[Index];
		
		// pop the value from input frame
		InFrame.Pop(&DataBuffer[CurrOffset], Property->ElementSize);
		CurrOffset += Property->ElementSize;
	}

	// pop the class reference and push it to the output frame
	uint8* ClassReference;
	InFrame.PopReference(ClassReference);	

	check(InFrame.GetTopOffset() == 0);

	// set the class reference
	SetClass(ClassReference);

	// add parameter in order
	CurrOffset = 0;
	for (HProperty* ParamProperty : InParameters)
	{
		// push the value to the output frame
		PushParameter(&DataBuffer[CurrOffset], ParamProperty->ElementSize);
		CurrOffset += ParamProperty->ElementSize;
	}
}

void HNativeFunctionFrame::GetOutput(uint8* OutData, int16& OutSize)
{
	HGenericMemory::MemCopy(OutData, &Storage[OutputOffset], OutputSize);
	OutSize = OutputSize;
}