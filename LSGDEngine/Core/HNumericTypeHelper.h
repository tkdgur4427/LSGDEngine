#pragma once

namespace lsgd { namespace reflect {

	// number

	template <class Type>
	struct IsNumberHelper
	{
		static const bool Value = false;
	};

	template <>
	struct IsNumberHelper<int8>
	{
		static const bool Value = true;
	};

	template <>
	struct IsNumberHelper<int16>
	{
		static const bool Value = true;
	};

	template <>
	struct IsNumberHelper<int32>
	{
		static const bool Value = true;
	};

	template <>
	struct IsNumberHelper<int64>
	{
		static const bool Value = true;
	};

	template <>
	struct IsNumberHelper<uint8>
	{
		static const bool Value = true;
	};

	template <>
	struct IsNumberHelper<uint16>
	{
		static const bool Value = true;
	};

	template <>
	struct IsNumberHelper<uint32>
	{
		static const bool Value = true;
	};

	template <>
	struct IsNumberHelper<uint64>
	{
		static const bool Value = true;
	};

	template <>
	struct IsNumberHelper<float>
	{
		static const bool Value = true;
	};

	template <>
	struct IsNumberHelper<double>
	{
		static const bool Value = true;
	};

	// boolean

	template <class Type>
	struct IsBooleanHelper
	{
		static const bool Value = false;
	};

	template <>
	struct IsBooleanHelper<bool>
	{
		static const bool Value = true;
	};

} }