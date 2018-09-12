#pragma once

namespace lsgd { namespace reflect {

	class HField
	{
	public:
		explicit HField(const HString& InName)
			: Name(InName)
		{}

		HString Name;
	};

	class HProperty : public HField
	{
	public:
		explicit HProperty(const HString& InName, int32 InOffset, int32 InElementSize, int32 InArrayDim = 1)
			: HField(InName)
			, ArrayDim(InArrayDim)
			, Offset(InOffset)
			, ElementSize(InElementSize)
			, FunctionInputNext(nullptr)
			, FunctionOutputNext(nullptr)
		{
			TotalSize = ElementSize * ArrayDim;
		}

		int32 ArrayDim;

		int32 Offset;
		int32 ElementSize;
		int32 TotalSize;

		// additional linked list node properties by its usage
		
		// if it is nullptr, it doesn't related to HFunction
		HProperty* FunctionInputNext;
		HProperty* FunctionOutputNext;

		//...
	};

	template <class ClassType, class FieldType>
	int32 StructOffsetOf(FieldType ClassType::* InClassField)
	{
		return (int8*)&((ClassType*)nullptr->*InClassField) - (int8*)nullptr;
	}

	class HStruct : public HField
	{
	public:
		HStruct(const HString& InName)
			: HField(InName)
		{}

		HStruct* SuperStruct;
				
	protected:
		friend class HTypeDatabase;

		void AddProperty(unique_ptr<HProperty>& InProperty);

		// properties
		HArray<unique_ptr<HProperty>> Properties;
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

	class HFunction : public HStruct
	{
	public:
		HFunction(const HString& InName)
			: HStruct(InName)
			, FunctionInputHead(nullptr)
			, FunctionOutputHead(nullptr)
		{}		

		virtual void CallFunction(void* InContext, const HFrame& InStack, void* const OutReturn) {}	

		uint8 ParamNum;
		uint16 ParamSize;
		uint16 ReturnValueOffset;

		// note that real-instance is managed by HStruct
		HProperty* FunctionInputHead;
		HProperty* FunctionOutputHead;
	};

	// forward declaration for function object
	class HNativeFunctionObject;

	// c++ native function object
	class HNativeFunction : public HFunction
	{
	public:
		explicit HNativeFunction(unique_ptr<HNativeFunctionObject>& InNativeFunctionObject);

		virtual void CallFunction(void* InContext, const HFrame& InStack, void* const OutReturn) override;

		// temporary
		HNativeFunctionObject* GetNativeFunctionObject() { return NativeFunctionObject.get(); }

	protected:
		// construct native function object
		void SetNativeFunctionObject(unique_ptr<HNativeFunctionObject>& InNativeFunctionObject);

		// native function object
		unique_ptr<HNativeFunctionObject> NativeFunctionObject;
	};

	// script function object
	class HScriptFunction : public HFunction
	{
	public:
	};

	struct HNativeFunctionLookup
	{
		HString Name;
		//NativeFunction Pointer;
	};

	class HClass : public HStruct
	{
	public:
		explicit HClass(const HString& InName)
			: HStruct(InName)
		{
		}

		void AddMethod(unique_ptr<HFunction>& InMethod);

		HClass* ClassWithin;
		void* ClassDefaultObject;

		hash_map<HString, HFunction*> FunctionMap;
		mutable hash_map<HString, HFunction*> SuperFunctionMap;

		HArray<HNativeFunctionLookup> NativeFunctionLookup;

		// real-container for method object
		HArray<unique_ptr<HFunction>> Methods;
	};

	class HEnum : public HField
	{
	public:
		HArray<pair<HString, int64>> Names;
		static hash_map<HString, HEnum*> AllEnumNames;
	};

	class HNumberProperty : public HProperty
	{
	public:
		HNumberProperty(const HString& InPrimitiveName, const HString& InVariableName, int32 InOffset, int32 InElementSize, int32 InArrayDim = 1)
			: HProperty(InVariableName, InOffset, InElementSize, InArrayDim)
			, PrimitiveName(InPrimitiveName)
		{}

		HString PrimitiveName;
	};

	class HBoolProperty : public HProperty
	{
	public:
		HBoolProperty(const HString& InVariableName, int32 InOffset, int32 InElementSize, int32 InArrayDim = 1)
			: HProperty(InVariableName, InOffset, InElementSize, InArrayDim)
		{}
	};

	class HStringProperty : public HProperty
	{
	public:
		HStringProperty(const HString& InVariableName, int32 InOffset, int32 InElementSize, int32 InArrayDim = 1)
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
