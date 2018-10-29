#pragma once
namespace lsgd {
	namespace reflect {

		// forward declaration
		class HReflectionContext;

	}
}

#define DECLARE_REFLECTION_CONTEXT_BASE_TYPE(Type) \
	extern lsgd::reflect::HReflectionContext& operator<<(lsgd::reflect::HReflectionContext& InContext, Type& InValue);