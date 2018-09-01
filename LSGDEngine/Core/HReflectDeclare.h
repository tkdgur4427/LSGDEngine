#pragma once

#include "HPrimitiveType.h"

// register primitive types
#define DECLARE_PRIMITIVE_TYPE(PrimitiveType)	\
	template <>	\
	class HPrimitiveTypeHelper<PrimitiveType>	\
	{	\
	public:	\
		static bool IsPrimitiveType() { return true; }	\
		static HGuid GetGuid() { static HGuid Guid = HGuid::CreateGuid(); return Guid;}	\
		static bool IsNumber() { return IsNumberHelper<PrimitiveType>::Value; } \
		static bool IsBoolean() { return IsBooleanHelper<PrimitiveType>::Value; } \
		static bool IsString() { return IsStringHelper<PrimitiveType>::Value; } \
	};	\
	class HStatic##PrimitiveType	\
	{	\
	public:	\
		HStatic##PrimitiveType();	\
	};	\
	extern HStatic##PrimitiveType Static##PrimitiveType; 

#define DECLARE_CLASS_TYPE(ClassType, SuperClassType)	\
	static lsgd::HString GetClassName() { return #ClassType; }	\
