#pragma once

#include "HVariadicStack.h"
using namespace lsgd::container;

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

	/*
		Universal HFunction's Frame
			- for script execution
			- @todo design & implement
	*/
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

	/*
		HFunction's direct stack frame
			- before implementing HFrame, native direct call frame is replaced by this frame structure
			- the layout for HDirectFunctionCallFrame (the order is important!)
			  ---------------------
			  Class Reference (if not class method, == 0)
			  Parameters
			  ReturnValue
			  ---------------------
	*/
	struct HDirectFunctionCallFrame : public HVariadicStack<256>
	{
		
	};

	class HFunction : public HStruct
	{
	public:
		HFunction(const HString& InName, const HStruct* InOwner = nullptr)
			: HStruct(InName)
			, Owner(InOwner)
			, FunctionInputHead(nullptr)
			, FunctionOutputHead(nullptr)
		{}

		virtual void CallFunction(void* InContext, const HFrame& InStack, void* const OutReturn) {}	

		// direct function call
		template <typename... ParamTypes>
		void Invoke(ParamTypes&&... Parameters);

		template <typename ClassType, typename... ParamTypes>
		void Invoke(ClassType* InClassValue, ParamTypes&&... InParams);

		// virtual method invoke call variation
		virtual void Invoke(HDirectFunctionCallFrame& InvokeFrame) {}

		// owner class
		//	- if == 0, it is just global or local function (not member function)
		const HStruct* const Owner;

		// function properties
		uint8 ParamNum;
		uint16 ParamSize;
		uint16 ReturnValueOffset;

		// note that real-instance is managed by HStruct
		HProperty* FunctionInputHead;
		HProperty* FunctionOutputHead;

	protected:
		template <typename... ParamTypes/*, int16... Indices*/>
		void SetParameters(HDirectFunctionCallFrame& RefFrame, ParamTypes&&... InParameters/*, HIntegerSequence<int16, Indices...>*/);

		// validate parameter and push the value into the frame
		template <typename ParamType>
		int16 PushParameter(HDirectFunctionCallFrame& RefFrame/*, int16 PropertyIndex*/, ParamType&& InParameter);
	};

	// forward declaration for function object
	class HNativeFunctionObject;

	// c++ native function object
	class HNativeFunction : public HFunction
	{
	public:
		explicit HNativeFunction(unique_ptr<HNativeFunctionObject>& InNativeFunctionObject, const HStruct* InOwner = nullptr);
				
		virtual void CallFunction(void* InContext, const HFrame& InStack, void* const OutReturn) override;
		virtual void Invoke(HDirectFunctionCallFrame& InvokeFrame);

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

	template <typename... ParamTypes>
	void HFunction::Invoke(ParamTypes&&... Parameters)
	{
		check(Owner == nullptr);
		return Invoke(nullptr, Parameters...);
	}

	template <typename... ParamTypes/*, int16... Indices*/>
	void HFunction::SetParameters(HDirectFunctionCallFrame& RefFrame, ParamTypes&&... InParameters/*, HIntegerSequence<int16, Indices...>*/)
	{
		// dummy for initializer list to trigger 'PushParameter'
		int16 DummyTriggerArray = { PushParameter(RefFrame/*, Indices*/, InParameters)... };
	}

	template <typename ClassType, typename... ParamTypes>
	void HFunction::Invoke(ClassType* InClassValue, ParamTypes&&... InParams)
	{
		HDirectFunctionCallFrame Frame;

		// insert class reference
		uint8* ClassReference = (uint8*)InClassValue;
		Frame.PushReference(ClassReference);

		// push parameters
		SetParameters(Frame, InParams.../*, HMakeIntegerSequence<int16, sizeof...(ParamTypes)>()*/);

		// invoke
		Invoke(Frame);
	}

	template <typename ParamType>
	int16 HFunction::PushParameter(HDirectFunctionCallFrame& RefFrame/*, int16 PropertyIndex*/, ParamType&& InParameter)
	{
		//HProperty* Property = FunctionInputHead;
		//for (int16 Index = 0; Index < PropertyIndex; ++Index)
		//{
		//	Property = Property->FunctionInputNext;
		//}
		//check(Property->ElementSize == sizeof(InParameter));

		RefFrame.PushByType<ParamType>(InParameter);

		return 0;
	}
}
}
