#pragma once

#include "HVariadicStack.h"
using namespace lsgd::container;

namespace lsgd { namespace reflect {

	class HTypeDescriptor;
	class HReflectionContext;

	class HField
	{
	public:
		explicit HField(const HString& InName)
			: Name(InName)
		{}
		virtual ~HField() {}

		HString Name;
	};

	class HProperty : public HField
	{
	public:
		enum
		{
			ReferenceSize = sizeof(void*),
		};

		/*
			InArrayDim: InArrayDim == 0; it means unbounded array (std::vector)
		*/
		explicit HProperty(const HString& InName, int32 InOffset, int32 InElementSize, int32 InArrayDim = 1)
			: HField(InName)
			, ArrayDim(InArrayDim)
			, Offset(InOffset)
			, ElementSize(InElementSize)
			, FunctionInputNext(nullptr)
			, FunctionOutputNext(nullptr)
			, bDynamicProperty(false)
			, bDynamicArray(InArrayDim == 0)
		{
			TotalSize = ElementSize * ArrayDim;
		}

		virtual ~HProperty() {}

		void SerializeItem(HReflectionContext& InContext, uint8* OutData);

		// type descriptor
		unique_ptr<HTypeDescriptor> TypeDescriptor;

		// dynamic container (HArray)
		//	- ArrayDim = 0 ; array dimension is unbounded
		bool bDynamicArray;

		// if ArrayDim == 0 && bDynamicArray; it is unbounded array (same as std::vector)
		int32 ArrayDim;

		int32 Offset;
		int32 ElementSize;
		int32 TotalSize;

		// whether the property is dynamic size or not
		bool bDynamicProperty;

		// to handling dynamic property, need one more indirection to point real data
		//	- note that only valid data structure, if bDynamicProperty == 1
		//	- at initialization time, it is not valid data
		//		- when it is going to serialize, it becomes valid
		struct HDynamicPropertyData
		{
			HDynamicPropertyData() 
				: Offset(-1), Size(-1)
			{}
			int32 Offset;
			int32 Size;
		};
		HDynamicPropertyData DynamicPropertyData;

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

		virtual ~HStruct() {}

		void SerializeProperties(HReflectionContext& InContext, uint8* OutData) const;

		HStruct* SuperStruct;
				
	protected:
		friend class HTypeDatabase;

		void AddProperty(unique_ptr<HProperty>& InProperty);		

		// properties
		HArray<unique_ptr<HProperty>> Properties;

		// children
		//	- HEnum, HStruct, ...
		HArray<unique_ptr<HField>> Children;		
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
		HDirectFunctionCallFrame()
			: HVariadicStack<256>()
			, OutputOffset(0)
			, OutputSize(0)
		{}

		void PushOutput(uint8* Output, int16 InSize)
		{
			int16 Offset = GetTopOffset();
			Push(Output, InSize);
			OutputOffset = Offset;
			OutputSize = InSize;
		}

		template <typename Type>
		Type PopOutput()
		{
			check(OutputSize == sizeof(Type));
			Type Output;
			Pop((uint8*)&Output, OutputSize);
			return Output;
		}

		int16 OutputOffset;
		int16 OutputSize;
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

		virtual ~HFunction() {}

		virtual void CallFunction(void* InContext, const HFrame& InStack, void* const OutReturn) {}	

		/*
			if HFunction is not member function, InClassValue should be nullptr
				- @todo need to make separate void* InClassValue deleted version
		*/
		template <typename ReturnType, typename... ParamTypes>
		ReturnType Invoke(void* InClassValue, ParamTypes&&... InParams);

		// virtual method invoke call variation
		virtual void InvokeInner(HDirectFunctionCallFrame& InvokeFrame) {}

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
		virtual ~HNativeFunction() {}

		virtual void CallFunction(void* InContext, const HFrame& InStack, void* const OutReturn) override;
		virtual void InvokeInner(HDirectFunctionCallFrame& InvokeFrame);

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
		virtual ~HScriptFunction() {}
	};

	class HClass : public HStruct
	{
	public:
		explicit HClass(const HString& InName)
			: HStruct(InName)
		{
		}
		virtual ~HClass() {}
		
		void AddMethod(unique_ptr<HFunction>& InMethod);

		// get the total size of HClass memory layout
		uint32 GetTotalSize() const;

		HClass* ClassWithin;
		void* ClassDefaultObject;

		HHashMap<HString, HFunction*> FunctionMap;
		mutable HHashMap<HString, HFunction*> SuperFunctionMap;

		// real-container for method object
		HArray<unique_ptr<HFunction>> Methods;
	};

	class HEnum : public HField
	{
	public:
		HArray<pair<HString, int64>> Names;
		static HHashMap<HString, HEnum*> AllEnumNames;
	};

	class HNumberProperty : public HProperty
	{
	public:
		HNumberProperty(const HString& InPrimitiveName, const HString& InVariableName, int32 InOffset, int32 InElementSize, int32 InArrayDim = 1)
			: HProperty(InVariableName, InOffset, InElementSize, InArrayDim)
		{}
		virtual ~HNumberProperty() {}
	};

	class HBoolProperty : public HProperty
	{
	public:
		HBoolProperty(const HString& InVariableName, int32 InOffset, int32 InElementSize, int32 InArrayDim = 1)
			: HProperty(InVariableName, InOffset, InElementSize, InArrayDim)
		{}
		virtual ~HBoolProperty() {}
	};

	class HStringProperty : public HProperty
	{
	public:
		HStringProperty(const HString& InVariableName, int32 InOffset, int32 InElementSize, int32 InArrayDim = 1)
			: HProperty(InVariableName, InOffset, InElementSize, InArrayDim)
		{
			// note that string property is dynamic property
			bDynamicProperty = true;
		}
		virtual ~HStringProperty() {}
	};

	class HEnumProperty : public HProperty
	{
	public:
		virtual ~HEnumProperty() {}

		HEnum* Enum;
		
		// note that actual enum property value is number
		HNumberProperty UnderlyingProperty;
	};

	class HClassProperty : public HProperty
	{
	public:
		HClassProperty(const HString& InVariableName, int32 InOffset, const HClass* InClass, int32 InArrayDim = 1)
			: HProperty(InVariableName, InOffset, ReferenceSize, InArrayDim)
			, Class(InClass)
		{
		}

		virtual ~HClassProperty() {}

		// class reflection 
		const HClass* Class;
	};

	class HStructProperty : public HProperty
	{
	public:
		virtual ~HStructProperty() {}

		HStruct* Struct;
	};

	template <typename... ParamTypes/*, int16... Indices*/>
	void HFunction::SetParameters(HDirectFunctionCallFrame& RefFrame, ParamTypes&&... InParameters/*, HIntegerSequence<int16, Indices...>*/)
	{
		// dummy for initializer list to trigger 'PushParameter'
		int16 DummyTriggerArray = { PushParameter(RefFrame/*, Indices*/, InParameters)... };
	}

	template <typename ReturnType, typename... ParamTypes>
	ReturnType HFunction::Invoke(void* InClassValue, ParamTypes&&... InParams)
	{
		HDirectFunctionCallFrame Frame;

		// insert class reference
		uint8* ClassReference = (uint8*)InClassValue;
		Frame.PushReference(ClassReference);

		// push parameters
		SetParameters(Frame, InParams.../*, HMakeIntegerSequence<int16, sizeof...(ParamTypes)>()*/);

		// invoke
		InvokeInner(Frame);

		return Frame.PopOutput<ReturnType>();
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
