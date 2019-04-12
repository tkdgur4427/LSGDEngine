#pragma once

#include <utility>

namespace lsgd {

	// @todo - need to implement own my own to replace std:: dependencies!

	// remove_reference
	template<class Type>
	using HRemoveReference = typename std::remove_reference<Type>;

	template <bool B, class T = void>
	using HEnableIf = std::enable_if<B, T>;

	template <class Type>
	struct HIsPointer
	{
		static const bool Value = false;
	};

	template <class Type>
	struct HIsPointer<Type*>
	{
		static const bool Value = true;
	};	

	template <class InType>
	struct HRemovePointer
	{
		using Type = InType;
	};

	template <class InType>
	struct HRemovePointer<InType*>
	{
		using Type = InType;
	};

	template <class T, class U>
	struct HIsSameType
	{
		static const bool Value = false;
	};

	template <class T>
	struct HIsSameType<T, T>
	{
		static const bool Value = true;
	};
}

namespace lsgd { namespace TMP {
	template <bool A, bool B>
	struct HAnd
	{
		static const bool Value = A & B;
	};

	template <bool A, bool B>
	struct HOr
	{
		static const bool Value = A | B;
	};
}
}