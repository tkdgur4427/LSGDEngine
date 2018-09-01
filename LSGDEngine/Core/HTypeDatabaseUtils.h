#pragma once

// note that the only header who can include 'HTypeDatabase.h'
#include "HTypeDatabase.h"

namespace lsgd { namespace reflect {

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

		// function properties
		bool IsClassFunction;
		bool IsConst;	// note that it is only valid when IsClassFunction == true
	};

	// function decomposer
	template <class FunctionType>
	struct HFunctionDecomposer {};

	// utility functions for HTypeDatabase, highly recommend to use this static methods
	class HTypeDatabaseUtils
	{
		template <typename Type>
		static HTypeDescriptor GetTypeDescriptor();

		static HTypeDescriptor GetTypeDescriptor(const HString& InTypeName);

		template <typename FunctionType>
		static HFunctionDecomposeResult DecomposeFunction();
	};

	template <typename Type>
	HTypeDescriptor HTypeDatabaseUtils::GetTypeDescriptor()
	{
		return GetTypeDescriptor(HTypeDatabase::GetSingleton()->GetTypeName<Type>());
	}

	template <typename FunctionType>
	static HFunctionDecomposeResult HTypeDatabaseUtils::DecomposeFunction()
	{
		return HFunctionDecomposer<FunctionType>.Decompose();
	}

	// function decomposer implementations

	template <typename ResultType, typename ...ParamTypes>
	struct HFunctionDecomposer<ResultType(*)(ParamTypes...)>
	{
		HFunctionDecomposeResult Decompose()
		{
			HFunctionDecomposeResult Result;
			Result.OutputType = HTypeDatabaseUtils::GetTypeDescriptor<ResultType>();
			
			if (sizeof...(ParamTypes) > 0)
			{
				// processing function input types
				auto TupleTypes = HMakeTuple(&HTypeDatabaseUtils::GetTypeDescriptor<ParamTypes>()...);
				auto ArrayTypes = ToFixedArray(TupleTypes);

				for (int32 Index = 0; Index < ArrayTypes.size(); ++Index)
				{
					Result.InputTypes.push_back(ArrayTypes[Index]);
				}
			}

			return Result;
		}
	};
}
}