#include "HCorePCH.h"
#include "HTypeDatabase.h"

// reflect
#include "HReflect.h"

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

void HTypeDatabase::AddClassType(const HString& InName, const HString& InSuperClassName)
{
	check(!ExistClass(InName));

	// declare the class instance
	unique_ptr<HClass> NewClass = lsgd::make_unique<HClass>(InName);
	Classes.push_back(move(NewClass));

	// generate class data for post-processing
	HPostProcessClassData NewPostprocessClassData;
	NewPostprocessClassData.ClassName = InName;
	NewPostprocessClassData.SuperClassName = InSuperClassName;

	PostProcessClassDataList.push_back(NewPostprocessClassData);
}

bool HTypeDatabase::ExistClass(const HString& InClassName)
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

int32 HTypeDatabase::GetClassIndex(const HString& InClassName)
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

const HClass* HTypeDatabase::GetClass(const HString& InClassName)
{
	return Classes[GetClassIndex(InClassName)].get();
}

void HTypeDatabase::LinkProperty(int32 InClassIndex, unique_ptr<HProperty>& InProperty)
{
	Classes[InClassIndex]->AddProperty(InProperty);
}

void HTypeDatabase::LinkMethod(int32 InClassIndex, unique_ptr<HNativeFunctionObject>& InNativeFunctionObject)
{
	unique_ptr<HFunction> NewFunction = make_unique<HFunction, HNativeFunction>(InNativeFunctionObject);
	Classes[InClassIndex]->AddMethod(NewFunction);
}

unique_ptr<HProperty> HTypeDatabase::CreatePrimitiveProperty(const HString& InTypeName, const HString& InVariableName, int32 InOffset, int32 InSize, int32 InArrayDim) const
{
	unique_ptr<HProperty> NewProperty;
	const HPrimitiveType* FoundType = GetPrimitiveType(InTypeName);

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
		// @todo...
		//NewProperty = make_shared<HProperty, HStringProperty>(InOffset, InSize, InArrayDim);
	}

	//...

	return NewProperty;
}

unique_ptr<HProperty> HTypeDatabase::CreatePropertyByName(const HString& InTypeName, const HString& InVariableName, int32 InOffset, int32 InSize, int32 InArrayDim) const
{
	// first handling primitive type
	if (ExistPrimitiveType(InTypeName))
	{
		return move(CreatePrimitiveProperty(InTypeName, InVariableName, InOffset, InSize, InArrayDim));
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
	: CurrOffset(0)
	, ClassRefOffset(0)
	, ClassRefSize(0)
	, OutputOffset(0)
	, OutputSize(0)
{
	// zeroify stack storage
	HGenericMemory::MemZero(&StackStorage[0], 0, sizeof(uint8) * StackSize);
}

void HNativeFunctionFrame::PushReference(uint8*& InData)
{
	int16 DataSize = sizeof(uint8*);
	check(CurrOffset + DataSize <= StackSize);

	//@todo - need to find fancy way
	uintptr_t CastedData = (uintptr_t)InData;

	HGenericMemory::MemCopy(&StackStorage[CurrOffset], &CastedData, DataSize);
	CurrOffset += DataSize;
}

void HNativeFunctionFrame::Push(uint8* Data, int16 DataSize)
{
	check(CurrOffset + DataSize <= StackSize);
	HGenericMemory::MemCopy(&StackStorage[CurrOffset], Data, DataSize);
	CurrOffset += DataSize;
}

void HNativeFunctionFrame::Pop(uint8* OutData, int16 DataSize)
{
	check(CurrOffset - DataSize >= 0);
	CurrOffset -= DataSize;
	HGenericMemory::MemCopy(OutData, &StackStorage[CurrOffset], DataSize);
}
