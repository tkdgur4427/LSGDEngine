#pragma once

namespace lsgd { namespace reflect {

	template <class Type>
	class HPrimitiveTypeHelper
	{
	public:
		static bool IsPrimitiveType() { return false; }
		static HGuid GetGuid() { static HGuid Guid; return Guid; }
	};

	enum EPrimitiveTypeFlags 
	{
		Number = (1 << 0),
		Boolean = (1 << 1),
		String = (1 << 2),
	};

	// primitive type interface
	class HPrimitiveType
	{
	public:
		HPrimitiveType(const HString& InName, const HGuid& InGuid, int32 InSize, int32 InFlags)
			: PrimitiveName(InName)
			, Guid(InGuid)
			, Size(InSize)
			, PrimitiveTypeFlags(InFlags)
		{}

		bool IsNumber() const { return (PrimitiveTypeFlags & EPrimitiveTypeFlags::Number) > 0; }
		bool IsBoolean() const { return (PrimitiveTypeFlags & EPrimitiveTypeFlags::Boolean) > 0; }
		bool IsString() const { return (PrimitiveTypeFlags & EPrimitiveTypeFlags::String) > 0; }

		HString PrimitiveName;
		HGuid Guid;
		int32 Size;

		int32 PrimitiveTypeFlags;
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
	struct IsStringHelper<HString>
	{
		static const bool Value = true;
	};
}
}