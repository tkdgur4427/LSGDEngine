#pragma once

#include "HTypeDatabase.h"

// note that it tries to register primitive type by static class instantiation
#define IMPLEMENT_PRIMITIVE_TYPE(PrimitiveType)	\
	HStatic##PrimitiveType::HStatic##PrimitiveType()	\
	{	\
		lsgd::reflect::HTypeDatabase::GetSingleton()->AddPrimitiveType<PrimitiveType>(#PrimitiveType);	\
	}	\
	HStatic##PrimitiveType Static##PrimitiveType;


#define IMPLEMENT_CLASS_TYPE(ClassType, SuperClassType)
