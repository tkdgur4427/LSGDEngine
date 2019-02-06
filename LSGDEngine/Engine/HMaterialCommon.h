#pragma once

#include "HGlobalShader.h"

namespace lsgd {

	// represents an expression which only varies with uniform inputs
	class HMaterialUniformExpression : public HRefCountedObject
	{
	public:
		//...
	};

	// stores all uniform for a material generated from a material translation
	class HUniformExpressionSet : public HRefCountedObject
	{
	public:
		HArray<HRefCountPtr<HMaterialUniformExpression>> UniformVectorExpressions;
		//...
	};

	// a shader meta type for material-linked shaders
	class HMaterialShaderType : public HShaderType
	{
	public:
		struct CompiledShaderInitializerType : HGlobalShaderType::CompiledShaderInitializerType
		{
			const HUniformExpressionSet& UniformExpressionSet;
			const HString DebugDescription;
		};
	};

	class HMaterialShaderMap : public HShaderMap<HMaterialShaderType>, public HDeferredCleanupInterface
	{
	public:
		virtual void FinishCleanup() override {}
	};

}