#pragma once

#include "HGlobalShader.h"

namespace lsgd
{
	class HImGuiVertexShader : public HGlobalShader
	{
		DECLARE_SHADER_TYPE(HImGuiVertexShader, Global);
	public:
		HImGuiVertexShader();
		HImGuiVertexShader(const CompiledShaderInitializerType& InInitializer);

		virtual ~HImGuiVertexShader() {}

		static bool ShouldCache(HShaderPlatform InShaderPlatform);
		static void ModifyCompilationEnvironment(HShaderPlatform InShaderPlatform, HShaderCompilerEnvironment& ShaderCompilerEnvironment);
	};

	class HImGuiPixelShader : public HGlobalShader
	{
		DECLARE_SHADER_TYPE(HImGuiPixelShader, Global);
	public:
		HImGuiPixelShader();
		HImGuiPixelShader(const CompiledShaderInitializerType& InInitializer);
		virtual ~HImGuiPixelShader() {}

		static bool ShouldCache(HShaderPlatform InShaderPlatform);
		static void ModifyCompilationEnvironment(HShaderPlatform InShaderPlatform, HShaderCompilerEnvironment& ShaderCompilerEnvironment);
	};
}

extern lsgd::HImGuiVertexShader* GImGuiVertexShader;
extern lsgd::HImGuiPixelShader* GImGuiPixelShader;
