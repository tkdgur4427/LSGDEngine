#pragma once

#include "HTypeDatabaseUtils.h"

namespace lsgd { namespace reflect {

	// native function implementation
	class HNativeFunctionObject
	{
	public:
		explicit HNativeFunctionObject(const HString& InName)
			: FunctionName(InName)
		{
			// call decompose function object (derived)
			DecomposeFunctionObject();
		}

		// decompose function object
		virtual void DecomposeFunctionObject() = 0;

		struct HFunctionInput
		{
			HString Name;
			HTypeDescriptor TypeDescriptor;
		};

		struct HFunctionOutput
		{
			HString Name;
			HTypeDescriptor TypeDescriptor;
		};

		// function name
		HString FunctionName;

		// function object properties
		HArray<HFunctionInput> FunctionInputs;
		HFunctionOutput FunctionOutput;		
	};

	template <typename Type>
	class HNativeFunctionObjectImpl : public HNativeFunctionObject
	{};

	// global function object or static function object
	template <typename ReturnType, typename ...ParamTypes>
	class HNativeFunctionObjectImpl<ReturnType(*)(ParamTypes...)> : public HNativeFunctionObject
	{
	public:
		typedef ReturnType (*FunctionPointerType) (ParamTypes...);

		explicit HNativeFunctionObjectImpl(const HString& InName, FunctionPointerType InFunction)
			: HNativeFunctionObject(InName)
			, FunctionPointer(InFunction)
		{}

		virtual void DecomposeFunctionObject() override {}

	protected:
		FunctionPointerType FunctionPointer;
	};

}
}