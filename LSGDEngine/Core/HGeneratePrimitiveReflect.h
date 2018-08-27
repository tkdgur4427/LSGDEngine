#pragma once

#include "HReflectDeclare.h"

// declare basic primitive types and register types
namespace lsgd { namespace reflect {

	DECLARE_PRIMITIVE_TYPE(int8)
	DECLARE_PRIMITIVE_TYPE(int16)
	DECLARE_PRIMITIVE_TYPE(int32)
	DECLARE_PRIMITIVE_TYPE(int64)

	DECLARE_PRIMITIVE_TYPE(uint8)
	DECLARE_PRIMITIVE_TYPE(uint16)
	DECLARE_PRIMITIVE_TYPE(uint32)
	DECLARE_PRIMITIVE_TYPE(uint64)

	DECLARE_PRIMITIVE_TYPE(bool)
	DECLARE_PRIMITIVE_TYPE(float)
	DECLARE_PRIMITIVE_TYPE(double)

	DECLARE_PRIMITIVE_TYPE(string)

}
}