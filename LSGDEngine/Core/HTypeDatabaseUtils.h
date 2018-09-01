#pragma once

// note that the only header who can include 'HTypeDatabase.h'
#include "HTypeDatabase.h"

namespace lsgd { namespace reflect {

	// utility functions for HTypeDatabase, highly recommend to use this static methods
	class HTypeDatabaseUtils
	{
	public:
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
		return HFunctionDecomposer<FunctionType>().Decompose();
	}
}
}