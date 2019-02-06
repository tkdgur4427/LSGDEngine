#pragma once

#include "HGlobalShader.h"

namespace lsgd
{
	class HImGuiVertexShader : public HGlobalShader
	{
		DECLARE_SHADER_TYPE(HImGuiVertexShader, Global);
	public:
		HImGuiVertexShader()
			: HGlobalShader()
		{}

		HImGuiVertexShader(const CompiledShaderInitializerType& InInitializer)
			: HGlobalShader(InInitializer)
		{}
		virtual ~HImGuiVertexShader() {}

		static bool ShouldCache(HShaderPlatform InShaderPlatform);
		static void ModifyCompilationEnvironment(HShaderPlatform InShaderPlatform, HShaderCompilerEnvironment& ShaderCompilerEnvironment);
	};

	class HImGuiPixelShader : public HGlobalShader
	{
		DECLARE_SHADER_TYPE(HImGuiPixelShader, Global);
	public:
		HImGuiPixelShader()
			: HGlobalShader()
		{}

		HImGuiPixelShader(const CompiledShaderInitializerType& InInitializer)
			: HGlobalShader(InInitializer)
		{}
		virtual ~HImGuiPixelShader() {}

		static bool ShouldCache(HShaderPlatform InShaderPlatform);
		static void ModifyCompilationEnvironment(HShaderPlatform InShaderPlatform, HShaderCompilerEnvironment& ShaderCompilerEnvironment);
	};
}
