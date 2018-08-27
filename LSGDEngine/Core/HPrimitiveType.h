#pragma once

namespace lsgd { namespace reflect {

	class HPrimitiveTypeImplBase {};

	template <class Type>
	class HPrimitiveTypeImpl : public HPrimitiveTypeImplBase
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
}
}