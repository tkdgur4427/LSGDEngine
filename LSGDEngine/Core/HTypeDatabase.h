#pragma once

#include "HCommonTypeHelper.h"
#include "HPrimitiveType.h"
#include "HClassType.h"
#include "HReflect.h"

// variadic stack
#include "HVariadicStack.h"
using namespace lsgd::container;

// hobject handle
#include "HObjectHandle.h"

namespace lsgd { namespace reflect {

	// type descriptor stored in HTypeDatabase
	class HTypeDescriptor
	{
	public:
		HTypeDescriptor()
			: PrimitiveType(nullptr)
			, ClassType(nullptr)
		{}

		int32 GetSize() const;
		unique_ptr<HProperty> CreateProperty(const HString& InVariableName, int32 InOffset = 0) const;

		// note that these type is just raw pointer (
		//	- all instances related to type is cached in HTypeDatabase and never be released!

		// if type is not primitive type, it will be nullptr;
		const class HPrimitiveType* PrimitiveType;

		// if type is not class type, it will be nullptr;
		const class HClass* ClassType;
	};

	// function decompose result
	struct HFunctionDecomposeResult
	{
		HFunctionDecomposeResult()
			: IsClassFunction(false)
			, IsConst(false)
		{}

		HTypeDescriptor OutputType;
		HArray<HTypeDescriptor> InputTypes;

		// only valid when IsClassFunction == true
		HTypeDescriptor ClassType;

		// function properties
		bool IsClassFunction;
		bool IsConst;	// note that it is only valid when IsClassFunction == true
	};

	// function decomposer
	struct HFunctionDecomposerBase
	{
		template <typename ReturnType>
		HTypeDescriptor ExtractOutputType() const;

		template <typename... ParamTypes>
		HArray<HTypeDescriptor> ExtractInputTypes() const;

		template <typename ClassType>
		HTypeDescriptor ExtractClassType() const;
	};

	template <class FunctionType>
	struct HFunctionDecomposer : public HFunctionDecomposerBase {};

	struct HDirectFunctionCallFrame;

	// for high-flexibility, separate native function stacks
	//	- the design is adapted from assembly process
	struct HNativeFunctionFrame : public HVariadicStack<128>
	{
		HNativeFunctionFrame();		

		// parameter offset to retrieve
		//	- @todo: we can optimize further by... something?
		HArray<int16> ParamOffsets;
		HArray<int16> ParamSizes;

		// class reference offset and size
		int16 ClassRefOffset;
		int16 ClassRefSize;

		// output offset and size
		int16 OutputOffset;
		int16 OutputSize;

		// set parameter by property
		void SetFrame(HDirectFunctionCallFrame& InFrame, HArray<HProperty*> InParameters);

		// set function frame data
		template <class... ParamTypes>
		void SetFrame(uint8* InClass, ParamTypes&&... Parameters);

		void PushParameter(uint8* InData, int16 InDataSize);

		void SetClass(uint8* InClassReference);

		// base function PushParamRecursive
		template <class ParamType>
		void PushParamRecursive(const ParamType& InParam);

		// template recursive function PushParamRecursive
		template <class ParamType, class... ParamTypes>
		void PushParamRecursive(const ParamType& InParam, ParamTypes&&... InRestParams);

		// push output param
		template <class OutputType>
		void PushOutput(const OutputType& InOutput);

		// getting parameter by template function
		template <typename Type>
		Type GetParameter(int32 Index) const;

		// getting class instance by template function
		template <typename ClassType>
		ClassType* GetClass() const;

		void GetOutput(uint8* OutData, int16& OutSize);

		// getting output
		template <class OutputType>
		OutputType GetOutput() const;
	};

	// native function implementation
	class HNativeFunctionObject
	{
	public:
		explicit HNativeFunctionObject(const HString& InName)
			: FunctionName(InName)
			, IsClassFunction(false)
			, IsConst(false)
		{
		}

		// process decomposed data
		void ProcessDecomposedData(const HFunctionDecomposeResult& InData);

		// common method (decomposing function object type)
		template <typename FunctionObjectType>
		void DecomposeFunctionObjectCommon();

		// decompose function object
		virtual void DecomposeFunctionObject() {};

		// call functions
		virtual void CallFunction(HNativeFunctionFrame& Frame) {}

		// utility functionalities
		HString GetClassName() const;

		struct HFunctionInput
		{
			HTypeDescriptor TypeDescriptor;
		};

		struct HFunctionOutput
		{
			HTypeDescriptor TypeDescriptor;
		};

		// function name
		HString FunctionName;

		// class type (only valid when IsClassFunction)
		HTypeDescriptor Class;

		// function object properties
		HArray<HFunctionInput> FunctionInputs;
		HFunctionOutput FunctionOutput;

		// function properties
		bool IsClassFunction;
		bool IsConst;
	};

	// base SFINAE for HNativeFunctionObjectImpl
	template <typename Type>
	class HNativeFunctionObjectImpl : public HNativeFunctionObject
	{
	protected:
	};

	// global function object or static function object
	template <typename ReturnType, typename ...ParamTypes>
	class HNativeFunctionObjectImpl<ReturnType(*)(ParamTypes...)> : public HNativeFunctionObject
	{
	public:
		typedef ReturnType(*FunctionPointerType) (ParamTypes...);

		explicit HNativeFunctionObjectImpl(const HString& InName, FunctionPointerType InFunction)
			: HNativeFunctionObject(InName)
			, FunctionPointer(InFunction)
		{}

		virtual void DecomposeFunctionObject() override;

		template <int16... Indices>
		void CallFunctionImpl(HNativeFunctionFrame& Frame, HIntegerSequence<int16, Indices...>);


		virtual void CallFunction(HNativeFunctionFrame& Frame) override;

	protected:
		FunctionPointerType FunctionPointer;
	};

	// class member method (non-const)
	template <typename ClassType, typename ReturnType, typename ...ParamTypes>
	class HNativeFunctionObjectImpl<ReturnType(ClassType::*)(ParamTypes...)> : public HNativeFunctionObject
	{
	public:
		typedef ReturnType(ClassType::*FunctionPointerType) (ParamTypes...);

		explicit HNativeFunctionObjectImpl(const HString& InName, FunctionPointerType InFunction)
			: HNativeFunctionObject(InName)
			, FunctionPointer(InFunction)
		{}

		virtual void DecomposeFunctionObject() override;

		template <int16... Indices>
		void CallFunctionImpl(HNativeFunctionFrame& Frame, HIntegerSequence<int16, Indices...>);

		virtual void CallFunction(HNativeFunctionFrame& Frame) override;

	protected:
		FunctionPointerType FunctionPointer;
	};

	// class member method (const)
	template <typename ClassType, typename ReturnType, typename ...ParamTypes>
	class HNativeFunctionObjectImpl<ReturnType(ClassType::*)(ParamTypes...) const> : public HNativeFunctionObject
	{
	public:
		typedef ReturnType(ClassType::*FunctionPointerType) (ParamTypes...) const;

		explicit HNativeFunctionObjectImpl(const HString& InName, FunctionPointerType InFunction)
			: HNativeFunctionObject(InName)
			, FunctionPointer(InFunction)
		{}

		virtual void DecomposeFunctionObject() override;

		template <int16... Indices>
		void CallFunctionImpl(HNativeFunctionFrame& Frame, HIntegerSequence<int16, Indices...>);

		virtual void CallFunction(HNativeFunctionFrame& Frame) override;

	protected:
		FunctionPointerType FunctionPointer;
	};

	// post-process class reflection data
	struct HPostProcessClassData
	{	
		HString ClassName;
		HString SuperClassName;
	};

	class HTypeDatabase
	{
	public:
		static HTypeDatabase* GetSingleton();

		template<typename Type>
		HString GetTypeName() const;

		template<class Type>
		HString GetClassName() const;

		template <class Type>
		void AddPrimitiveType(const HString& InName);

		bool ExistPrimitiveType(const HString& InName) const;
		HString FindPrimitiveTypeName(const HGuid& InGuid) const;
		const HPrimitiveType* GetPrimitiveType(const HString& InName) const;

		template <typename Type>
		unique_ptr<HProperty> CreatePrimitiveProperty(const HString& InVariableName, int32 InOffset, int32 InSize, int32 InArrayDim = 1) const;

		unique_ptr<HProperty> CreatePrimitiveProperty(const HString& InTypeName, const HString& InVariableName, int32 InOffset, int32 InSize, int32 InArrayDim = 1) const;

		template <typename ClassType>
		unique_ptr<HProperty> CreateClassProperty(const HString& InVariableName, int32 InOffset, int32 InArrayDim = 1) const;

		unique_ptr<HProperty> CreateClassProperty(const HString& InClassTypeName, const HString& InVariableName, int32 InOffset, int32 InArrayDim = 1) const;

		template <typename Type>
		unique_ptr<HProperty> CreatePropertyByType(const HString& InVariableName, int32 InOffset, int32 InSize, int32 InArrayDim = 1) const;

		unique_ptr<HProperty> CreatePropertyByName(const HString& InTypeName, const HString& InVariableName, int32 InOffset, int32 InSize, int32 InArrayDim = 1) const;

		template <typename ObjectType>
		unique_ptr<HProperty> CreateObjectProperty(const HString& InVariableName, int32 InOffset, int32 InArrayDim = 1) const;

		template <class Type>
		void AddClassType(const HString& InName, const HString& InSuperClassName);

		// process post process class data
		void ProcessPostProcessClassData();

		void AddClassTypeInner(const HString& InName, const HString& InSuperClassName);
		bool ExistClass(const HString& InClassName) const;
		int32 GetClassIndex(const HString& InClassName) const;
		const HClass* GetClass(const HString& InClassName) const;
		HClass* GetClass(const HString& InClassName);
		const HCommonTypeHelperInterface* GetClassCommonTypeHelper(const HString& InClassName) const;
		HCommonTypeHelperInterface* GetClassCommonTypeHelper(const HString& InClassName);

		template <class ClassType, class FieldType>
		void AddClassField(const HString& InFieldName, FieldType ClassType::*InField);

		template <class ClassType, class FieldType>
		void AddClassField(const HString& InFieldName, HArray<FieldType> ClassType::*InField);

		template <class ClassType, class FieldType, uint32 N>
		void AddClassField(const HString& InFieldName, FieldType(ClassType::*InField)[N]);

		template <class ClassMethodType>
		void AddClassMethod(const HString& InMethodName, ClassMethodType InMethod);

		// generated class types
		HArray<unique_ptr<HClass>> Classes;

		// class type helper
		HArray<HCommonTypeHelperInterface*> ClassCommonTypeHelpers;

		// temporary data - after processing class data, it will removes all data
		HArray<HPostProcessClassData> PostProcessClassDataList;

		// primitive types
		HArray<unique_ptr<HPrimitiveType>> PrimitiveTypes;

		// primitive type helper
		HArray<HCommonTypeHelperInterface*> PrimitiveTypeCommonTypeHelpers;

		// primitive type mapper by guid
		HHashMap<HGuid, int32> GuidToPrimitiveTypes;

		// primitive type mapper by name
		HHashMap<HString, int32> NameToPrimitiveTypes;

	protected:
		HTypeDatabase() {}

		// methods to cut the dependency with HReflect.h
		void LinkProperty(int32 InClassIndex, unique_ptr<HProperty>& InProperty);

		void LinkMethod(int32 InClassIndex, unique_ptr<HNativeFunctionObject>& InNativeFunctionObject);
	};

	// utility functions for HTypeDatabase, highly recommend to use this static methods
	class HTypeDatabaseUtils
	{
	public:
		template <typename Type>
		static HTypeDescriptor GetTypeDescriptor();

		static HTypeDescriptor GetTypeDescriptor(const HString& InTypeName);

		static const HCommonTypeHelperInterface* GetClassCommonTypeHelper(const HString& InTypeName);

		template <typename FunctionType>
		static HFunctionDecomposeResult DecomposeFunction();

		template <typename ClassType>
		static bool ExistClass();
	};

	// function decomposer implementations

	template <typename ReturnType>
	HTypeDescriptor HFunctionDecomposerBase::ExtractOutputType() const
	{
		return HTypeDatabaseUtils::template GetTypeDescriptor<ReturnType>();
	}

	template <typename... ParamTypes>
	HArray<HTypeDescriptor> HFunctionDecomposerBase::ExtractInputTypes() const
	{
		HArray<HTypeDescriptor> Result;
		if (sizeof...(ParamTypes) > 0)
		{
			// processing function input types
			auto TupleTypes = HMakeTuple(&HTypeDatabaseUtils::template GetTypeDescriptor<ParamTypes>()...);
			auto ArrayTypes = ToFixedArray<HTypeDescriptor>(TupleTypes);

			for (uint32 Index = 0; Index < ArrayTypes.size(); ++Index)
			{
				Result.push_back(ArrayTypes[Index]);
			}
		}

		return Result;
	}

	template <typename ClassType>
	HTypeDescriptor HFunctionDecomposerBase::ExtractClassType() const
	{
		/*
			ISO C++03 14.2/4
				- when the name of a member template specialization appears after .or -> in a postfix expression, or after
				nested-name-specifier in a qualified-id, and the postfix-expression or qualified-id explicitly depends on 
				a template-parameter, the member template name must be prefixed by the keyword template
				otherwise, the name is assumed to name a non-template
		*/

		return HTypeDatabaseUtils::template GetTypeDescriptor<ClassType>();
	}

	template <typename ResultType, typename ...ParamTypes>
	struct HFunctionDecomposer<ResultType(*)(ParamTypes...)> : public HFunctionDecomposerBase
	{
		HFunctionDecomposeResult Decompose()
		{
			HFunctionDecomposeResult Result;
			Result.OutputType = ExtractOutputType<ResultType>();
			Result.InputTypes = ExtractInputTypes<ParamTypes...>();

			return Result;
		}
	};

	template <typename ClassType, typename ResultType, typename ...ParamTypes>
	struct HFunctionDecomposer<ResultType(ClassType::*)(ParamTypes...)> : public HFunctionDecomposerBase
	{
		HFunctionDecomposeResult Decompose()
		{
			HFunctionDecomposeResult Result;
			Result.OutputType = ExtractOutputType<ResultType>();
			Result.InputTypes = ExtractInputTypes<ParamTypes...>();
			Result.ClassType = ExtractClassType<ClassType>();

			Result.IsClassFunction = true;

			return Result;
		}
	};

	template <typename ClassType, typename ResultType, typename ...ParamTypes>
	struct HFunctionDecomposer<ResultType(ClassType::*)(ParamTypes...) const> : public HFunctionDecomposerBase
	{
		HFunctionDecomposeResult Decompose()
		{
			HFunctionDecomposeResult Result;
			Result.OutputType = ExtractOutputType<ResultType>();
			Result.InputTypes = ExtractInputTypes<ParamTypes...>();
			Result.ClassType = ExtractClassType<ClassType>();

			Result.IsClassFunction = true;
			Result.IsConst = true;

			return Result;
		}
	};

	template <class ParamType>
	void HNativeFunctionFrame::PushParamRecursive(const ParamType& InParam)
	{
		// record the parameter size & offset
		int16 Offset = GetTopOffset();
		int16 Size = sizeof(ParamType);
		
		ParamOffsets.push_back(Offset);
		ParamSizes.push_back(Size);

		// push to the stack frame
		PushByType(InParam);
	}

	template <class ParamType, class... ParamTypes>
	void HNativeFunctionFrame::PushParamRecursive(const ParamType& InParam, ParamTypes&&... InRestParams)
	{
		// call base function
		PushParamRecursive(InParam);

		// call push parameter recursively
		PushParamRecursive(InRestParams...);
	}

	template <class OutputType>
	void HNativeFunctionFrame::PushOutput(const OutputType& InOutput)
	{
		OutputOffset = GetTopOffset();
		OutputSize = sizeof(OutputType);

		PushByType(InOutput);
	}
		
	template <class... ParamTypes>
	void HNativeFunctionFrame::SetFrame(uint8* InClass, ParamTypes&&... Parameters)
	{
		// insert class instance pointer
		PushReference(InClass);

		// record class reference
		ClassRefOffset = 0;
		ClassRefSize = GetTopOffset();

		// insert parameters
		int32 ParamNum = sizeof...(Parameters);
		if (ParamNum)
		{
			// push parameter compile-time recursively
			PushParamRecursive(Parameters...);
		}
	}
		
	template <typename Type>
	Type HNativeFunctionFrame::GetParameter(int32 Index) const
	{
		check(Index < (int32)ParamOffsets.size());

		int16 Offset = ParamOffsets[Index];
		int16 Size = ParamSizes[Index];
		check(Size == sizeof(Type));

		Type Result;
		HGenericMemory::MemCopy((uint8*)&Result, &Storage[Offset], Size);
		return Result;
	}
		
	template <typename ClassType>
	ClassType* HNativeFunctionFrame::GetClass() const
	{
		ClassType* ClassReference = nullptr;
		
		uintptr_t ClassPtr;
		HGenericMemory::MemCopy((uint8*)&ClassPtr, &Storage[ClassRefOffset], ClassRefSize);
		ClassReference = (ClassType*)ClassPtr;

		return ClassReference;
	}

	template <class OutputType>
	OutputType HNativeFunctionFrame::GetOutput() const
	{
		OutputType Result;
		HGenericMemory::MemCopy((uint8*)&Result, &Storage[OutputOffset], OutputSize);
		return Result;
	}

	template <typename FunctionObjectType>
	void HNativeFunctionObject::DecomposeFunctionObjectCommon()
	{
		HFunctionDecomposeResult Result;
		Result = HFunctionDecomposer<FunctionObjectType>().Decompose();

		ProcessDecomposedData(Result);
	}

	template <typename ReturnType, typename ...ParamTypes>
	void HNativeFunctionObjectImpl<ReturnType(*)(ParamTypes...)>::DecomposeFunctionObject()
	{
		DecomposeFunctionObjectCommon<FunctionPointerType>();
	}

	template <typename ReturnType, typename... ParamTypes>
	template <int16... Indices>
	void HNativeFunctionObjectImpl<ReturnType(*)(ParamTypes...)>::CallFunctionImpl(HNativeFunctionFrame& Frame, HIntegerSequence<int16, Indices...>)
	{
		// call class method
		ReturnType Result = (*FunctionPointer)(Frame.GetParameter<ParamTypes>(Indices)...);

		// push output to the frame
		Frame.PushOutput(Result);
	}

	template <typename ReturnType, typename ...ParamTypes>
	void HNativeFunctionObjectImpl<ReturnType(*)(ParamTypes...)>::CallFunction(HNativeFunctionFrame& Frame)
	{
		CallFunctionImpl(Frame, HMakeIntegerSequence<int16, sizeof...(ParamTypes)>());
	}

	template <typename ClassType, typename ReturnType, typename ...ParamTypes>
	void HNativeFunctionObjectImpl<ReturnType(ClassType::*)(ParamTypes...)>::DecomposeFunctionObject()
	{
		DecomposeFunctionObjectCommon<FunctionPointerType>();
	}

	template <typename ClassType, typename ReturnType, typename... ParamTypes>
	template <int16... Indices>
	void HNativeFunctionObjectImpl<ReturnType(ClassType::*)(ParamTypes...)>::CallFunctionImpl(HNativeFunctionFrame& Frame, HIntegerSequence<int16, Indices...>)
	{
		// get the class instance
		ClassType* ClassInstance = Frame.GetClass<ClassType>();

		// call class method
		ReturnType Result = (ClassInstance->*FunctionPointer)(Frame.GetParameter<ParamTypes>(Indices)...);

		// push output to the frame
		Frame.PushOutput(Result);
	}

	template <typename ClassType, typename ReturnType, typename ...ParamTypes>
	void HNativeFunctionObjectImpl<ReturnType(ClassType::*)(ParamTypes...)>::CallFunction(HNativeFunctionFrame& Frame)
	{
		CallFunctionImpl(Frame, HMakeIntegerSequence<int16, sizeof...(ParamTypes)>());
	}

	template <typename ClassType, typename ReturnType, typename ...ParamTypes>
	void HNativeFunctionObjectImpl<ReturnType(ClassType::*)(ParamTypes...) const>::DecomposeFunctionObject()
	{
		DecomposeFunctionObjectCommon<FunctionPointerType>();
	}

	template <typename ClassType, typename ReturnType, typename... ParamTypes>
	template <int16... Indices>
	void HNativeFunctionObjectImpl<ReturnType(ClassType::*)(ParamTypes...) const>::CallFunctionImpl(HNativeFunctionFrame& Frame, HIntegerSequence<int16, Indices...>)
	{
		// get the class instance
		ClassType* ClassInstance = Frame.GetClass<ClassType>();

		// call class method
		ReturnType Result = (ClassInstance->*FunctionPointer)(Frame.GetParameter<ParamTypes>(Indices)...);

		// push output to the frame
		Frame.PushOutput(Result);
	}

	template <typename ClassType, typename ReturnType, typename ...ParamTypes>
	void HNativeFunctionObjectImpl<ReturnType(ClassType::*)(ParamTypes...) const>::CallFunction(HNativeFunctionFrame& Frame)
	{
		CallFunctionImpl(Frame, HMakeIntegerSequence<int16, sizeof...(ParamTypes)>());
	}

	// template method implementations

	template<typename Type>
	HString HTypeDatabase::GetTypeName() const
	{
		if (HPrimitiveTypeHelper<Type>::IsPrimitiveType())
		{
			return FindPrimitiveTypeName(HPrimitiveTypeHelper<Type>::GetGuid());
		}

		if (HClassTypeHelper<Type>::IsClassType())
		{
			return GetClassName<Type>();
		}		
		
		return HString();
	}

	template<class Type>
	HString HTypeDatabase::GetClassName() const
	{
		check(HClassTypeHelper<Type>::IsClassType());
		return Type::GetClassName();
	}

	template <class Type>
	void HTypeDatabase::AddPrimitiveType(const HString& InName)
	{
		check(!ExistPrimitiveType(InName));

		int32 PrimitiveTypeFlags = 0;
		PrimitiveTypeFlags = HPrimitiveTypeHelper<Type>::IsNumber() ? (PrimitiveTypeFlags | EPrimitiveTypeFlags::Number) : PrimitiveTypeFlags;
		PrimitiveTypeFlags = HPrimitiveTypeHelper<Type>::IsBoolean() ? (PrimitiveTypeFlags | EPrimitiveTypeFlags::Number) : PrimitiveTypeFlags;
		PrimitiveTypeFlags = HPrimitiveTypeHelper<Type>::IsString() ? (PrimitiveTypeFlags | EPrimitiveTypeFlags::Number) : PrimitiveTypeFlags;

		unique_ptr<HPrimitiveType> NewPrimitiveType = lsgd::make_unique<HPrimitiveType>(InName, HPrimitiveTypeHelper<Type>::GetGuid(), sizeof(Type), PrimitiveTypeFlags);

		// create dynamic properties for HString
		if ((PrimitiveTypeFlags & EPrimitiveTypeFlags::String) != 0)
		{			
			// note that currently, only supporting char string!
			NewPrimitiveType->DynamicProperties.Size = HDynamicPrimitiveProperties::CountSize + sizeof(char);
		}

		int32 NewIndex = (int32)PrimitiveTypes.size();

		// add mapper from guid to primitive type
		GuidToPrimitiveTypes.insert({ NewPrimitiveType->Guid, NewIndex });

		// add mapper from name to primitive type
		NameToPrimitiveTypes.insert({ NewPrimitiveType->PrimitiveName, NewIndex });

		PrimitiveTypes.push_back(HMove(NewPrimitiveType));

		// add common type helper
		PrimitiveTypeCommonTypeHelpers.push_back(HPrimitiveTypeHelper<Type>::GetCommonTypeHelper());
	}

	template <class Type>
	void HTypeDatabase::AddClassType(const HString& InName, const HString& InSuperClassName)
	{
		// add class type inner
		AddClassTypeInner(InName, InSuperClassName);
		// add common type helper
		ClassCommonTypeHelpers.push_back(HClassTypeHelper<Type>::GetCommonTypeHelper());
	}

	template <typename Type>
	unique_ptr<HProperty> HTypeDatabase::CreatePrimitiveProperty(const HString& InVariableName, int32 InOffset, int32 InSize, int32 InArrayDim) const
	{
		unique_ptr<HProperty> NewProperty;
		if (HPrimitiveTypeHelper<Type>::IsNumber())
		{
			HString PrimitiveTypeName = FindPrimitiveTypeName(HPrimitiveTypeHelper<Type>::GetGuid());
			NewProperty = make_unique<HProperty, HNumberProperty>(PrimitiveTypeName, InVariableName, InOffset, InSize, InArrayDim);
		}
		else if (HPrimitiveTypeHelper<Type>::IsBoolean())
		{
			NewProperty = make_unique<HProperty, HBoolProperty>(InVariableName, InOffset, InSize, InArrayDim);
		}
		else if (HPrimitiveTypeHelper<Type>::IsChar())
		{
			NewProperty = make_unique<HProperty, HCharProperty>(InVariableName, InOffset, InSize, InArrayDim);
		}
		else if (HPrimitiveTypeHelper<Type>::IsString())
		{
			NewProperty = make_unique<HProperty, HStringProperty>(InVariableName, InOffset, InSize, InArrayDim);
		}

		//...

		return NewProperty;
	}

	template <typename ClassType>
	unique_ptr<HProperty> HTypeDatabase::CreateClassProperty(const HString& InVariableName, int32 InOffset, int32 InArrayDim) const
	{
		const HString ClassTypeName = HClassTypeHelper<ClassType>::GetClassName();
		return HMove(CreateClassProperty(ClassTypeName, InVariableName, InOffset, InArrayDim));
	}

	template <typename Type>
	unique_ptr<HProperty> HTypeDatabase::CreatePropertyByType(const HString& InVariableName, int32 InOffset, int32 InSize, int32 InArrayDim) const
	{
		// first handling primitive type
		if (HPrimitiveTypeHelper<Type>::IsPrimitiveType())
		{
			return HMove(CreatePrimitiveProperty<Type>(InVariableName, InOffset, InSize, InArrayDim));
		}

		// second handling class type
		else if (HClassTypeHelper<Type>::IsClassType())
		{
			return HMove(CreateClassProperty<Type>(InVariableName, InOffset, InArrayDim));
		}

		return nullptr;
	}

	template <typename ObjectType>
	unique_ptr<HProperty> HTypeDatabase::CreateObjectProperty(const HString& InVariableName, int32 InOffset, int32 InArrayDim) const
	{
		unique_ptr<HProperty> NewProperty = make_unique<HProperty, HObjectProperty>(InVariableName, InOffset, sizeof(HObjectHandleUnique<HObject>), InArrayDim);
		return HMove(NewProperty);
	}

	template <class ClassType, class FieldType>
	void HTypeDatabase::AddClassField(const HString& InFieldName, FieldType ClassType::*InField)
	{
		HString ClassName = ClassType::GetClassName();
		check(ExistClass(ClassName));

		int32 ClassIndex = GetClassIndex(ClassName);
		int32 FieldOffset = (int32)StructOffsetOf(InField);
		int32 FieldSize = sizeof(FieldType);

		unique_ptr<HProperty> NewProperty;
		if (HPrimitiveTypeHelper<FieldType>::IsPrimitiveType())
		{
			NewProperty = HMove(CreatePropertyByType<FieldType>(InFieldName, FieldOffset, FieldSize));
		}
		else if (HIsObjectHandleUnique<FieldType>::Value)
		{
			using ObjectType = typename HRemoveObjectHandleUnique<FieldType>::Type;
			NewProperty = HMove(CreateObjectProperty<ObjectType>(InFieldName, FieldOffset));
		}
		else if (HClassTypeHelper<FieldType>::IsClassType())
		{
			check(ExistClass(HClassTypeHelper<FieldType>::GetClassName()));

			FieldSize = GetClassCommonTypeHelper(HClassTypeHelper<FieldType>::GetClassName())->GetSize();
			NewProperty = HMove(CreatePropertyByType<FieldType>(InFieldName, FieldOffset, FieldSize));
		}
		else
		{
			check(0);
		}

		LinkProperty(ClassIndex, NewProperty);
	}

	template <class ClassType, class FieldType, uint32 N>
	void HTypeDatabase::AddClassField(const HString& InFieldName, FieldType(ClassType::*InField)[N])
	{
		HString ClassName = ClassType::GetClassName();
		check(ExistClass(ClassName));

		int32 ClassIndex = GetClassIndex(ClassName);
		int32 FieldOffset = (int32)StructOffsetOf(InField);
		int32 FieldSize = sizeof(FieldType);

		unique_ptr<HProperty> NewProperty;
		if (HPrimitiveTypeHelper<FieldType>::IsPrimitiveType())
		{
			NewProperty = HMove(CreatePropertyByType<FieldType>(InFieldName, FieldOffset, FieldSize, N));
		}
		else if (HIsObjectHandleUnique<FieldType>::Value)
		{
			using ObjectType = typename HRemoveObjectHandleUnique<FieldType>::Type;
			NewProperty = HMove(CreateObjectProperty<ObjectType>(InFieldName, FieldOffset));
		}
		else if (HClassTypeHelper<FieldType>::IsClassType())
		{
			check(ExistClass(HClassTypeHelper<FieldType>::GetClassName()));

			FieldSize = GetClassCommonTypeHelper(HClassTypeHelper<FieldType>::GetClassName())->GetSize();
			NewProperty = HMove(CreatePropertyByType<FieldType>(InFieldName, FieldOffset, FieldSize));
		}
		else
		{
			check(0);
		}

		LinkProperty(ClassIndex, NewProperty);
	}

	template <class ClassType, class FieldType>
	void HTypeDatabase::AddClassField(const HString& InFieldName, HArray<FieldType> ClassType::*InField)
	{
		HString ClassName = ClassType::GetClassName();
		check(ExistClass(ClassName));

		int32 ClassIndex = GetClassIndex(ClassName);
		int32 FieldOffset = (int32)StructOffsetOf(InField);
		int32 FieldSize = sizeof(FieldType);
				
		unique_ptr<HProperty> NewProperty;
		if (HPrimitiveTypeHelper<FieldType>::IsPrimitiveType())
		{
			NewProperty = HMove(CreatePropertyByType<FieldType>(InFieldName, FieldOffset, FieldSize, 0));
		}
		else if (HIsObjectHandleUnique<FieldType>::Value)
		{
			using ObjectType = typename HRemoveObjectHandleUnique<FieldType>::Type;
			NewProperty = HMove(CreateObjectProperty<ObjectType>(InFieldName, FieldOffset, 0));
		}
		else if (HClassTypeHelper<FieldType>::IsClassType())
		{
			check(ExistClass(HClassTypeHelper<FieldType>::GetClassName()));

			FieldSize = GetClassCommonTypeHelper(HClassTypeHelper<FieldType>::GetClassName())->GetSize();
			NewProperty = HMove(CreatePropertyByType<FieldType>(InFieldName, FieldOffset, FieldSize, 0));
		}
		else
		{
			check(0);
		}
		
		LinkProperty(ClassIndex, NewProperty);
	}

	template <class ClassMethodType>
	void HTypeDatabase::AddClassMethod(const HString& InMethodName, ClassMethodType InMethod)
	{
		unique_ptr<HNativeFunctionObject> NewNativeFunctionObject = lsgd::make_unique<HNativeFunctionObject, HNativeFunctionObjectImpl<ClassMethodType>>(InMethodName, InMethod);
		NewNativeFunctionObject->DecomposeFunctionObject();

		int32 ClassIndex = GetClassIndex(NewNativeFunctionObject->GetClassName());
		LinkMethod(ClassIndex, NewNativeFunctionObject);
	}

	template <typename Type>
	HTypeDescriptor HTypeDatabaseUtils::GetTypeDescriptor()
	{
		return GetTypeDescriptor(HTypeDatabase::GetSingleton()->GetTypeName<Type>());
	}

	template <typename FunctionType>
	static HFunctionDecomposeResult HTypeDatabaseUtils::DecomposeFunction()
	{
		return HFunctionDecomposer<FunctionType>().Decompose();
	}

	template <typename ClassType>
	bool HTypeDatabaseUtils::ExistClass()
	{
		return GetTypeDescriptor<ClassType>().ClassType != nullptr;
	}
}
}
