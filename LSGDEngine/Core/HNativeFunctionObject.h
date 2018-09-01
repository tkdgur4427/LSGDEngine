#pragma once

#include "HTypeDatabaseUtils.h"

namespace lsgd { namespace reflect {

	// native function implementation
	class HNativeFunctionObject
	{
	public:
		explicit HNativeFunctionObject(const HString& InName)
			: FunctionName(InName)
			, IsClassFunction(false)
			, IsConst(false)
		{
			// call decompose function object (derived)
			DecomposeFunctionObject();
		}

		// common method (decomposing function object type)
		template <typename FunctionObjectType>
		void DecomposeFunctionObjectCommon();

		// decompose function object
		virtual void DecomposeFunctionObject() = 0;

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
		typedef ReturnType (*FunctionPointerType) (ParamTypes...);

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

	template <typename FunctionObjectType>
	void HNativeFunctionObject::DecomposeFunctionObjectCommon()
	{
		HFunctionDecomposeResult Result;
		Result = HTypeDatabaseUtils::DecomposeFunction<FunctionPointerType>();

		IsClassFunction = Result.IsClassFunction;
		IsConst = Result.IsConst;

		FunctionOutput.TypeDescriptor = Result.OutputType;
		for (HTypeDescriptor& Descriptor : Result.InputTypes)
		{
			FunctionInputs.push_back({ Descriptor });
		}
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
}
}