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
		typedef void (*ModifyCompilationEnvironmentType)(HShaderPlatform, const class HMaterial*, class HShaderCompilerEnvironment&);
		typedef bool (*SupportsTessellationShaderType)();

		HString Name;
		HString ShaderFilename;
		uint32 bUsedWithMaterials : 1;
		uint32 bSupportsStaticLighting : 1;
		uint32 bSupportsDynamicLighting : 1;
		uint32 bSupportsPrecisePreviewWorldPos : 1;
		uint32 bSupportsPositionOnly : 1;

		ConstructParameterType ConstructParameters;
		ShouldCacheType ShouldCacheRef;
		ModifyCompilationEnvironmentType ModifyCompilationEnvironmentRef;
		SupportsTessellationShaderType SupportsTessellationShadersRef;

		/*
			cache of referenced uniform buffer includes
			these are derived from source files so they need to be flushed when editing and recompiling shaders on the fly
			HVertexFactoryType::Initialize will add an entry for each referenced uniform buffer, but the declarations are added on demand as shaders are compiled
		*/
		HHashMap<HString, HCachedUniformBufferDeclaration> ReferencedUniformStructsCache;

		// tracks what platforms ReferencedUniformBufferStructsCache has had declarations cached for
		bool bCachedUniformBufferStructDeclarations[HShaderPlatform::SP_NumPlatforms];
	};
}