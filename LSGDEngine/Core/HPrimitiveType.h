#pragma once

namespace lsgd { namespace reflect {

	template <class Type>
	class HPrimitiveTypeHelper
	{
	public:
		static bool IsPrimitiveType() { return false; }
		static HGuid GetGuid() { static HGuid Guid; return Guid; }
	};

	// primitive type interface
	class HPrimitiveType
	{
	public:
		HPrimitiveType(const string& InName, const HGuid& InGuid)
			: PrimitiveName(InName)
			, Guid(InGuid)
		{}

		string PrimitiveName;
		HGuid Guid;
	};	

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

	// string

	template <class Type>
	struct IsStringHelper
	{
		static const bool Value = false;
	};

	template <>
	struct IsStringHelper<string>
	{
		static const bool Value = true;
	};
}
}