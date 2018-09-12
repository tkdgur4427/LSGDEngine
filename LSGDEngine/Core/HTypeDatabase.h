#pragma once

#include "HPrimitiveType.h"

namespace lsgd { namespace reflect {

	// forward declaration
	class HProperty;
	class HClass;

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

	// for high-flexibility, separate native function stacks
	//	- the design is adapted from assembly process
	struct HNativeFunctionFrame
	{
		enum 
		{
			StackSize = 128, // maximum 16 entries allowed (supposing each entry has 8 bytes)
		};

		uint8 StackStorage[StackSize];
		int16 CurrOffset;

		// parameter offset to retrieve
		//	- @todo: we can optimize further by... something?
		HArray<int16> ParamOffsets;
		HArray<int16> ParamSizes;

		HNativeFunctionFrame();

		// set function frame data
		template <class... ParamTypes>
		void SetFrame(uint8* InClass, ParamTypes&&... Parameters);

		// base function PushParamRecursive
		template <class ParamType>
		void PushParamRecursive(const ParamType& InParam);

		// template recursive function PushParamRecursive
		template <class ParamType, class... ParamTypes>
		void PushParamRecursive(const ParamType& InParam, ParamTypes&&... InRestParams);

		// getting parameter by template function
		template <typename Type>
		Type GetParameter(int32 Index);

		// getting class instance by template function
		template <typename ClassType>
		ClassType GetClass();

	protected:
		// methods
		//	- currently really simple management for stack frame for native function
		//	- if we need, we need to make more solid way
		template <typename Type>
		void PushByType(const Type& InValue);

		template <typename Type>
		Type PopByType();

		void PushReference(uint8*& InData);
		void Push(uint8* Data, int16 DataSize);
		void Pop(uint8* OutData, int16 DataSize);

		uint16 GetTopOffset() const { return CurrOffset; }
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
		//void CallFunction(HNativeFunctionFrame& Frame, )

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

		template <class Type>
		void AddPrimitiveType(const HString& InName);

		bool ExistPrimitiveType(const HString& InName) const;
		HString FindPrimitiveTypeName(const HGuid& InGuid) const;
		const HPrimitiveType* GetPrimitiveType(const HString& InName) const;

		template <typename Type>
		unique_ptr<HProperty> CreatePrimitiveProperty(const HString& InVariableName, int32 InOffset, int32 InSize, int32 InArrayDim = 1) const;

		unique_ptr<HProperty> CreatePrimitiveProperty(const HString& InTypeName, const HString& InVariableName, int32 InOffset, int32 InSize, int32 InArrayDim = 1) const;

		template <typename Type>
		unique_ptr<HProperty> CreatePropertyByType(const HString& InVariableName, int32 InOffset, int32 InSize, int32 InArrayDim = 1) const;

		unique_ptr<HProperty> CreatePropertyByName(const HString& InTypeName, const HString& InVariableName, int32 InOffset, int32 InSize, int32 InArrayDim = 1) const;

		void AddClassType(const HString& InName, const HString& InSuperClassName);
		bool ExistClass(const HString& InClassName);
		int32 GetClassIndex(const HString& InClassName);
		const HClass* GetClass(const HString& InClassName);

		template <class ClassType, class FieldType>
		void AddClassField(const HString& InFieldName, FieldType ClassType::*InField);

		template <class ClassMethodType>
		void AddClassMethod(const HString& InMethodName, ClassMethodType InMethod);

		// generated class types
		HArray<unique_ptr<HClass>> Classes;

		// temporary data - after processing class data, it will removes all data
		HArray<HPostProcessClassData> PostProcessClassDataList;

		// primitive types
		HArray<unique_ptr<HPrimitiveType>> PrimitiveTypes;

		// primitive type mapper by guid
		hash_map<HGuid, int32> GuidToPrimitiveTypes;

		// primitive type mapper by name
		hash_map<HString, int32> NameToPrimitiveTypes;

	protected:
		HTypeDatabase() {}

		// methods to cut the dependency with HReflect.h
		void LinkProperty(int32 InClassIndex, unique_ptr<HProperty>& InProperty);

		void LinkMethod(int32 InClassIndex, unique_ptr<HNativeFunctionObject>& InNativeFunctionObject);
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

	template <typename Type>
	void HNativeFunctionFrame::PushByType(const Type& InValue)
	{
		Push((uint8*)&InValue, sizeof(Type));
	}

	template <typename Type>
	Type HNativeFunctionFrame::PopByType()
	{
		Type Result;
		Pop((uint8*)&Result, sizeof(Type));
		return Result;
	}

	template <class ParamType>
	void HNativeFunctionFrame::PushParamRecursive(const ParamType& InParam)
	{
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
		
	template <class... ParamTypes>
	void HNativeFunctionFrame::SetFrame(uint8* InClass, ParamTypes&&... Parameters)
	{
		// insert class instance pointer
		PushReference(InClass);

		// insert parameters
		int32 ParamNum = sizeof...(Parameters);
		if (ParamNum)
		{
			// push parameter compile-time recursively
			PushParamRecursive(Parameters...);
		}
	}
		
	template <typename Type>
	Type HNativeFunctionFrame::GetParameter(int32 Index)
	{

	}
		
	template <typename ClassType>
	ClassType HNativeFunctionFrame::GetClass()
	{

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

	template <typename ClassType, typename ReturnType, typename ...ParamTypes>
	void HNativeFunctionObjectImpl<ReturnType(ClassType::*)(ParamTypes...)>::DecomposeFunctionObject()
	{
		DecomposeFunctionObjectCommon<FunctionPointerType>();
	}

	template <typename ClassType, typename ReturnType, typename ...ParamTypes>
	void HNativeFunctionObjectImpl<ReturnType(ClassType::*)(ParamTypes...) const>::DecomposeFunctionObject()
	{
		DecomposeFunctionObjectCommon<FunctionPointerType>();
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
			return HClassTypeHelper<Type>::template GetClassName();
		}		
		
		return HString();
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

		int32 NewIndex = (int32)PrimitiveTypes.size();

		// add mapper from guid to primitive type
		GuidToPrimitiveTypes.insert({ NewPrimitiveType->Guid, NewIndex });

		// add mapper from name to primitive type
		NameToPrimitiveTypes.insert({ NewPrimitiveType->PrimitiveName, NewIndex });

		PrimitiveTypes.push_back(move(NewPrimitiveType));
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
		else if (HPrimitiveTypeHelper<Type>::IsString())
		{
			// @todo...
			//NewProperty = make_shared<HProperty, HStringProperty>(InOffset, InSize, InArrayDim);
		}

		//...

		return NewProperty;
	}

	template <typename Type>
	unique_ptr<HProperty> HTypeDatabase::CreatePropertyByType(const HString& InVariableName, int32 InOffset, int32 InSize, int32 InArrayDim) const
	{
		// first handling primitive type
		if (HPrimitiveTypeHelper<Type>::IsPrimitiveType())
		{
			return move(CreatePrimitiveProperty<Type>(InVariableName, InOffset, InSize, InArrayDim));
		}

		// second ...		
		return nullptr;
	}

	template <class ClassType, class FieldType>
	void HTypeDatabase::AddClassField(const HString& InFieldName, FieldType ClassType::*InField)
	{
		HString ClassName = ClassType::GetClassName();
		check(ExistClass(ClassName));

		int32 ClassIndex = GetClassIndex(ClassName);
		int32 FieldOffset = (int32)StructOffsetOf(InField);
		int32 FieldSize = sizeof(FieldType);

		// create property
		unique_ptr<HProperty> NewProperty = move(CreatePropertyByType<FieldType>(InFieldName, FieldOffset, FieldSize));
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
}
}
