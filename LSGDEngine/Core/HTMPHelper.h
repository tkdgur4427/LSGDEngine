#pragma once

#include <utility>

namespace lsgd {

	// @todo - need to implement own my own to replace std:: dependencies!

	// remove_reference
	template<class Type>
	using HRemoveReference = typename std::remove_reference_t<Type>::type;

	template <bool B, class T = void>
	using HEnableIf = std::enable_if<B, T>;
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