#pragma once

namespace lsgd
{
	// an interface to the parameter bindings for the VF used by a shader
	class HVertexFactoryShaderParameters
	{
	public:
	};

	// an object used to represent the type of a VF
	class HVertexFactoryType
	{
	public:
		typedef HVertexFactoryShaderParameters* (*ConstructParameterType)(HShaderFrequency ShaderFrequency);
		typedef bool (*ShouldCacheType)(HShaderPlatform, const class HMaterial*, const class HShaderType*);
		typedef void (*ModifyCompilationEnvironmentType)(HShaderPlatform, const class HMaterial*, )
	};
}