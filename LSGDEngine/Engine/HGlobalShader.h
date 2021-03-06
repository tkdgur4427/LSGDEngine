#pragma once

#include "HShader.h"
#include "HShaderCompilerEnvironment.h"

namespace lsgd
{
	/*
		a shader meta type for the simplest shaders; shaders which are not material or vertex factory linked
		there should only a single instance of each simple shader type
	*/
	class HGlobalShaderType : public HShaderType
	{
	public:
		typedef HShader::CompiledShaderInitializerType CompiledShaderInitializerType;
		typedef HShader* (*ConstructCompiledType)(const CompiledShaderInitializerType&);
		typedef bool (*ShouldCacheType)(HShaderPlatform);
		typedef void (*ModifyCompilationEnvironmentType)(HShaderPlatform, HShaderCompilerEnvironment&);

		HGlobalShaderType(const HString& InSourceFilename, const HString& InFunctionName, int32 InFrequency, ConstructSerializedType ConstructSerialized, ConstructCompiledType ConstructCompiled, ShouldCacheType ShouldCache, ModifyCompilationEnvironmentType ModifyCompilationEnvironment);
		virtual ~HGlobalShaderType();

		// set up the environment used to compile an instance of this shader type
		void SetupCompileEnvironment(HShaderPlatform Platform, HShaderCompilerEnvironment& Environment)
		{
			ModifyCompilationEnvironmentRef(Platform, Environment);
		}

		ConstructCompiledType ConstructCompiledRef;
		ShouldCacheType ShouldCacheRef;
		ModifyCompilationEnvironmentType ModifyCompilationEnvironmentRef;
	};

	class HGlobalShader : public HShader
	{
		//DECLARE_SHADER_TYPE(HGlobalShader, Global);
	public:
		HGlobalShader()
			: HShader(CompiledShaderInitializerType())
		{}

		HGlobalShader(const CompiledShaderInitializerType& InInitializer)
			: HShader(InInitializer)
		{}
		virtual ~HGlobalShader() {}
	};

	// class that encapsulates logic to create a DDC key for the global shader map
	class HGlobalShaderMapId
	{
	public:
		HGlobalShaderMapId(HShaderPlatform InPlatform);

		// shader types that this shader map is dependent on and their stored state
		HArray<HShaderTypeDependency> ShaderTypeDependencies;

		// shader pipeline types that this shader map is dependent on and their stored state
		HArray<HShaderPipelineTypeDependency> ShaderPipelineTypeDependencies;
	};
}

extern unique_ptr<lsgd::HShaderMap<lsgd::HGlobalShaderType>> GGlobalShaderMap[HShaderPlatform::SP_NumPlatforms];