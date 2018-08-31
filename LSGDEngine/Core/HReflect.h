#pragma once

namespace lsgd { namespace reflect {

	class HPrimitiveTypeBase;

	class HField
	{
	public:
		explicit HField(const string& InName)
			: Name(InName)
		{}

		string Name;
	};

	class HProperty : public HField
	{
	public:
		explicit HProperty(const string& InName, int32 InOffset, int32 InElementSize, int32 InArrayDim = 1)
			: HField(InName)
			, ArrayDim(InArrayDim)
			, Offset(InOffset)
			, ElementSize(InElementSize)			
		{
			TotalSize = ElementSize * ArrayDim;
		}

		int32 ArrayDim;

		int32 Offset;
		int32 ElementSize;
		int32 TotalSize;
	};

	template <class ClassType, class FieldType>
	int32 StructOffsetOf(FieldType ClassType::* InClassField)
	{
		return (int8*)&((ClassType*)nullptr->*InClassField) - (int8*)nullptr;
	}

	class HStruct : public HField
	{
	public:
		HStruct(const string& InName)
			: HField(InName)
		{}

		HStruct* SuperStruct;
				
	protected:
		friend class HTypeDatabase;

		void AddProperty(unique_ptr<HProperty>& InProperty);

		// properties
		vector<unique_ptr<HProperty>> Properties;
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
		explicit HClass(const string& InName)
			: HStruct(InName)
		{
		}

		HClass* ClassWithin;
		void* ClassDefaultObject;

		hash_map<string, HFunction> FunctionMap;
		mutable hash_map<string, HFunction> SuperFunctionMap;

		vector<HNativeFunctionLookup> NativeFunctionLookup;
	};

	class HEnum : public HField
	{
	public:
		vector<pair<string, int64>> Names;
		static hash_map<string, HEnum*> AllEnumNames;
	};

	class HNumberProperty : public HProperty
	{
	public:
		HNumberProperty(const string& InPrimitiveName, const string& InVariableName, int32 InOffset, int32 InElementSize, int32 InArrayDim = 1)
			: HProperty(InVariableName, InOffset, InElementSize, InArrayDim)
			, PrimitiveName(InPrimitiveName)
		{}

		string PrimitiveName;
	};

	class HBoolProperty : public HProperty
	{
	public:
		HBoolProperty(const string& InVariableName, int32 InOffset, int32 InElementSize, int32 InArrayDim = 1)
			: HProperty(InVariableName, InOffset, InElementSize, InArrayDim)
		{}
	};

	class HStringProperty : public HProperty
	{
	public:
		HStringProperty(const string& InVariableName, int32 InOffset, int32 InElementSize, int32 InArrayDim = 1)
			: HProperty(InVariableName, InOffset, InElementSize, InArrayDim)
		{}
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
