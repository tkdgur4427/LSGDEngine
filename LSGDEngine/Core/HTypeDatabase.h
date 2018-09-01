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

		template <typename Type>
		unique_ptr<HProperty> CreatePropertyByType(const HString& InVariableName, int32 InOffset, int32 InSize, int32 InArrayDim = 1) const;

		void AddClassType(const HString& InName, const HString& InSuperClassName);
		bool ExistClass(const HString& InClassName);
		int32 GetClassIndex(const HString& InClassName);
		const HClass* GetClass(const HString& InClassName);

		template <class ClassType, class FieldType>
		void AddClassField(const HString& InFieldName, FieldType ClassType::*InField);

		template <class ClassType, class ReturnType, class... ParamTypes>
		void AddClassMethod(const HString& InMethodName, ReturnType(ClassType::*InMethod)(ParamTypes...));

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
	};

	// function decomposer implementations

	template <typename ReturnType>
	HTypeDescriptor HFunctionDecomposerBase::ExtractOutputType() const
	{
		return HTypeDatabaseUtils::GetTypeDescriptor<ReturnType>();
	}

	template <typename... ParamTypes>
	HArray<HTypeDescriptor> HFunctionDecomposerBase::ExtractInputTypes() const
	{
		HArray<HTypeDescriptor> Result;
		if (sizeof...(ParamTypes) > 0)
		{
			// processing function input types
			auto TupleTypes = HMakeTuple(&HTypeDatabaseUtils::GetTypeDescriptor<ParamTypes>()...);
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
		return HTypeDatabaseUtils::GetTypeDescriptor<ClassType>();
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

	// template method implementations

	template<typename Type>
	HString HTypeDatabase::GetTypeName() const
	{
		if (HPrimitiveTypeHelper<Type>::IsPrimitiveType())
		{
			return FindPrimitiveTypeName(HPrimitiveTypeHelper<Type>::GetGuid());
		}
		
		// by default, fall into getting class name (@todo fix this)
		//return Type::GetClassName();

		return HString();
	}

	template <class Type>
	void HTypeDatabase::AddPrimitiveType(const HString& InName)
	{
		check(!ExistPrimitiveType(InName));

		unique_ptr<HPrimitiveType> NewPrimitiveType = lsgd::make_unique<HPrimitiveType>(InName, HPrimitiveTypeHelper<Type>::GetGuid());

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

	template <class ClassType, class ReturnType, class... ParamTypes>
	void HTypeDatabase::AddClassMethod(const HString& InMethodName, ReturnType(ClassType::*InMethod)(ParamTypes...))
	{
		HFunctionDecomposeResult Result = HFunctionDecomposer<decltype(InMethod)>().Decompose();
	}
}
}
