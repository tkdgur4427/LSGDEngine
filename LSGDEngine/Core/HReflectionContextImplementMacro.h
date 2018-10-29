#pragma once

#include "HReflectionContext.h"

#define IMPLEMENT_REFLECTION_CONTEXT_BASE_TYPE(Type) \
	lsgd::reflect::HReflectionContext& operator<<(lsgd::reflect::HReflectionContext& InContext, Type& InValue) \
	{ \
		InContext.Serialize((void*)&InValue, sizeof(InValue)); \
		return InContext; \
	}
