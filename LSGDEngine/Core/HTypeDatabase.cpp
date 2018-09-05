#include "HCorePCH.h"
#include "HTypeDatabase.h"

// reflect
#include "HReflect.h"

using namespace lsgd;
using namespace lsgd::reflect;

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

void HNativeFunctionObject::ProcessDecomposedData(const HFunctionDecomposeResult& InData)
{
	IsClassFunction = InData.IsClassFunction;
	IsConst = InData.IsConst;

	FunctionOutput.TypeDescriptor = InData.OutputType;
	for (const HTypeDescriptor& Descriptor : InData.InputTypes)
	{
		FunctionInputs.push_back({ Descriptor });
	}
}
