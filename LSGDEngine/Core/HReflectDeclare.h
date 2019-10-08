#pragma once

#include "HPrimitiveType.h"
#include "HClassType.h"
#include "HCommonTypeHelper.h"

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
		static bool IsStaticArray() { return false; } \
		static HCommonTypeHelperInterface* GetCommonTypeHelper() { static HCommonTypeHelper<PrimitiveType> CommonTypeHelper; return &CommonTypeHelper; } \
	};	\
	class HStatic##PrimitiveType	\
	{	\
	public:	\
		HStatic##PrimitiveType();	\
	};	\
	extern HStatic##PrimitiveType Static##PrimitiveType;

#define GENERATE_CLASS_BODY(ClassType) \
	static lsgd::HString GetClassName() { return #ClassType; } \
	static void Reflect(); \

// @todo need to support multiple base classes
#define DECLARE_CLASS_TYPE(ClassType, ...) \
	class ClassType; \
	template <> \
	struct HClassTypeHelper<ClassType> \
	{ \
		static bool IsClassType() { return true; } \
		static lsgd::HString GetClassName() { return #ClassType; } \
		static lsgd::HString GetBaseClassNames() { return lsgd::HString(); } \
		static lsgd::reflect::HCommonTypeHelperInterface* GetCommonTypeHelper() { static lsgd::reflect::HCommonClassTypeHelper<ClassType> CommonTypeHelper; return &CommonTypeHelper; } \
	}; \
	class HStatic##ClassType \
	{ \
	public: \
		HStatic##ClassType(); \
	}; \
	extern HStatic##ClassType Static##ClassType;
	
// supporting only one base class inheritance
#define DECLARE_CLASS_TYPE1(ClassType, BaseClassType) \
	class ClassType; \
	class BaseClassType; \
	template <> \
	struct HClassTypeHelper<ClassType> \
	{ \
		using Super = BaseClassType; \
		static bool IsClassType() { return true; } \
		static lsgd::HString GetClassName() { return #ClassType; } \
		static lsgd::HString GetBaseClassNames() { return #BaseClassType; } \
		static lsgd::reflect::HCommonTypeHelperInterface* GetCommonTypeHelper() { static lsgd::reflect::HCommonClassTypeHelper<ClassType> CommonTypeHelper; return &CommonTypeHelper; } \
	}; \
	class HStatic##ClassType \
	{ \
	public: \
		HStatic##ClassType(); \
	}; \
	extern HStatic##ClassType Static##ClassType; 