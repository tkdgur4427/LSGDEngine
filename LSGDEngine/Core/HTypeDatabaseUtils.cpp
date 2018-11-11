#include "HCorePCH.h"
#include "HTypeDatabaseUtils.h"

using namespace lsgd;
using namespace lsgd::reflect;

HTypeDescriptor HTypeDatabaseUtils::GetTypeDescriptor(const HString& InTypeName)
{
	HTypeDescriptor Result;

	// first search from primitive type
	if (HTypeDatabase::GetSingleton()->ExistPrimitiveType(InTypeName))
	{
		Result.PrimitiveType = HTypeDatabase::GetSingleton()->GetPrimitiveType(InTypeName);
	}

	// get class type
	else if (HTypeDatabase::GetSingleton()->ExistClass(InTypeName))
	{
		Result.ClassType = HTypeDatabase::GetSingleton()->GetClass(InTypeName);
	}

	// ...
	//else if 

	return Result;
}

const HCommonTypeHelperInterface* HTypeDatabaseUtils::GetClassCommonTypeHelper(const HString& InTypeName)
{
	return HTypeDatabase::GetSingleton()->GetClassCommonTypeHelper(InTypeName);
}
