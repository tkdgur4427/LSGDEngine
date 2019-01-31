#pragma once

#include "HShader.h"

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
		typedef void(*ModifyCompilationEnvironmentType)(HShaderPlatform, HShaderCompilerEnvironment&);

		ConstructCompiledType ConstructCompiledRef;
		ShouldCacheType ShouldCacheRef;
		ModifyCompilationEnvironmentType ModifyCompilationEnvironmentRef;
	};

	class HGlobalShader : public HShader
	{
		DECLARE_SHADER_TYPE(HGlobalShader, Global);
	public:
	};
}