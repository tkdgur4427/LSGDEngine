#pragma once

typedef signed char        int8;
typedef short              int16;
typedef int                int32;
typedef long long          int64;
typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
typedef unsigned long long uint64;

// forward declaration
namespace lsgd {
	namespace reflect {

		class HReflectionContext;

	}
}

// helper extern method
extern lsgd::reflect::HReflectionContext SerializeHelper(lsgd::reflect::HReflectionContext& InContext, void* InPointer, int64 InSize);

#include "HNumericTypeHelper.h"

template <typename Type, typename = lsgd::HEnableIf<lsgd::TMP::HOr<lsgd::reflect::HPrimitiveTypeHelper<Type>::IsNumber(), lsgd::reflect::HPrimitiveTypeHelper<Type>::IsBoolean()>::Value>>
lsgd::reflect::HReflectionContext& operator<<(lsgd::reflect::HReflectionContext& InContext, Type& Value)
{
	SerializeHelper(InContext, &Value, sizeof(Value));
	return InContext;
}
