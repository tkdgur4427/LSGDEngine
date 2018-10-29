#pragma once

namespace lsgd { namespace reflect {

	// string
	template <class Type>
	struct IsStringHelper
	{
		static const bool Value = false;
	};

	template <>
	struct IsStringHelper<lsgd::HString>
	{
		static const bool Value = true;
	};

}
}