#pragma once

#include "HPrimitiveType.h"

// register primitive types
#define DECLARE_PRIMITIVE_TYPE(PrimitiveType)	\
	template <>	\
	class HPrimitiveTypeImpl<PrimitiveType> : public HPrimitiveTypeImplBase	\
	{	\
	public:	\
		static bool IsPrimitiveType() { return true; }	\
		static HGuid GetGuid() { static HGuid Guid = HGuid::CreateGuid(); return Guid;}	\
	};	\
	class HStatic##PrimitiveType	\
	{	\
	public:	\
		HStatic##PrimitiveType();	\
	};	\
	extern HStatic##PrimitiveType Static##PrimitiveType; 

#define DECLARE_CLASS_TYPE(ClassType, SuperClassType)	\
	static lsgd::string GetClassName() { return #ClassType; }	\
