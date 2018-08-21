#pragma once

namespace lsgd { namespace reflect {

	class HField
	{
	public:
		HField* Next;
		string Name;
	};

	class HProperty : public HField
	{
	public:
		int32 ArrayDim;

		// sibling property link
		HProperty* PropertyLink;
	};

	class HStruct : public HField
	{
	public:
		HStruct* SuperStruct;

		// struct's property link head
		HProperty* PropertyLink;
	};

	class HFunction;

	class HReturnParam
	{
	public:
		HProperty* Property;
		uint8* PropertyAddress;

		HReturnParam* Next;
	};

	class HFrame
	{
	public:
		HFunction* Node;
		void* Context;
		uint8* Code;
		uint8* Locals;

		HFrame* PrevFame;
		HReturnParam* ReturnParam;
	};

	typedef void (*NativeFunction)(void* InContext, const HFrame& InStack, void* const OutReturn);

	class HFunction : public HStruct
	{
	public:
		NativeFunction Func;
		uint8 ParamNum;
		uint16 ParamSize;
		uint16 ReturnValueOffset;
	};

	struct HNativeFunctionLookup
	{
		string Name;
		NativeFunction Pointer;
	};

	class HClass : public HStruct
	{
	public:
		HClass* ClassWithin;
		void* ClassDefaultObject;

		hash_map<string, HFunction> FunctionMap;
		mutable hash_map<string, HFunction> SuperFunctionMap;

		array<HNativeFunctionLookup> NativeFunctionLookup;
	};

	class HEnum : public HField
	{
	public:
		array<pair<string, int64>> Names;
		static hash_map<string, HEnum*> AllEnumNames;
	};

	class HNumberProperty : public HProperty
	{
	public:
	};

	class HBoolProperty : public HProperty
	{
	public:
	};

	class HEnumProperty : public HProperty
	{
	public:
		HEnum* Enum;
		HNumberProperty* UnderlyingProperty;
	};

	class HClassProperty : public HProperty
	{
	public:
		HClass* Class;
	};

	class HStructProperty : public HProperty
	{
	public:
		HStruct* Struct;
	};
}
}
