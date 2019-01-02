#pragma once

#include "HNumericTypeHelper.h"
#include "HStringTypeHelper.h"

namespace lsgd { namespace reflect {

	template <class Type>
	class HPrimitiveTypeHelper
	{
	public:
		static bool IsPrimitiveType() { return false; }
		static HGuid GetGuid() { static HGuid Guid; return Guid; }
		static bool IsNumber() { return false; }
		static bool IsBoolean() { return false; }
		static bool IsString() { return false; }
		static class HCommonTypeHelperInterface* GetCommonTypeHelper() { return nullptr; }
	};

	enum EPrimitiveTypeFlags 
	{
		Number = (1 << 0),
		Boolean = (1 << 1),
		String = (1 << 2),
	};

	// handling dynamic size for primitive types (not static types)
	struct HDynamicPrimitiveProperties
	{
		enum { CountSize = sizeof(int64), };

		int64 GetTypeSize() const { return Size - CountSize; }

		// sizeof(Type) + CountSize
		// if Type is 'char', 1 byte + 8 bytes (for int64)
		int64 Size;
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

		int64 GetTotalSize() const { if (IsString()) { return DynamicProperties.Size; } return (int64)Size; }
		int32 GetTypeSize() const { if (IsString()) { return (int32)DynamicProperties.GetTypeSize(); } return Size; }

		HString PrimitiveName;
		HGuid Guid;
		int32 PrimitiveTypeFlags;

		union 
		{
			int32 Size;

			/*
				used for HString currently
			*/
			HDynamicPrimitiveProperties DynamicProperties;
		};
	};	
}
}