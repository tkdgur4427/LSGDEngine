#include "HEnginePCH.h"
#include "HImGuiShaders.h"

using namespace lsgd;

lsgd::HImGuiVertexShader* GImGuiVertexShader = nullptr;;
lsgd::HImGuiPixelShader* GImGuiPixelShader = nullptr;

IMPLEMENT_SHADER_TYPE(, HImGuiVertexShader, "HImGuiVS.hlsl", "main", HShaderFrequency::SF_Vertex)

HImGuiVertexShader::HImGuiVertexShader()
	: HGlobalShader()
{
	GImGuiVertexShader = this;
}

HImGuiVertexShader::HImGuiVertexShader(const CompiledShaderInitializerType& InInitializer)
	: HGlobalShader(InInitializer)
{
	GImGuiVertexShader = this;
}

bool HImGuiVertexShader::ShouldCache(HShaderPlatform InShaderPlatform)
{
	return true;
}

void HImGuiVertexShader::ModifyCompilationEnvironment(HShaderPlatform InShaderPlatform, HShaderCompilerEnvironment& ShaderCompilerEnvironment)
{

}

IMPLEMENT_SHADER_TYPE(, HImGuiPixelShader, "HImGuiPS.hlsl", "main", HShaderFrequency::SF_Pixel)

HImGuiPixelShader::HImGuiPixelShader()
	: HGlobalShader()
{
	GImGuiPixelShader = this;
}

HImGuiPixelShader::HImGuiPixelShader(const CompiledShaderInitializerType& InInitializer)
	: HGlobalShader(InInitializer)
{
	GImGuiPixelShader = this;
}

bool HImGuiPixelShader::ShouldCache(HShaderPlatform InShaderPlatform)
{
	return true;
}

void HImGuiPixelShader::ModifyCompilationEnvironment(HShaderPlatform InShaderPlatform, HShaderCompilerEnvironment& ShaderCompilerEnvironment)
{

}