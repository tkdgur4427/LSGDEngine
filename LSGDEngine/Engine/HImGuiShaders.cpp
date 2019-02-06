#include "HEnginePCH.h"
#include "HImGuiShaders.h"

using namespace lsgd;

IMPLEMENT_SHADER_TYPE(, HImGuiVertexShader, HImGuiVS, main, HShaderFrequency::SF_Vertex)

bool HImGuiVertexShader::ShouldCache(HShaderPlatform InShaderPlatform)
{
	return true;
}

void HImGuiVertexShader::ModifyCompilationEnvironment(HShaderPlatform InShaderPlatform, HShaderCompilerEnvironment& ShaderCompilerEnvironment)
{

}

IMPLEMENT_SHADER_TYPE(, HImGuiPixelShader, "HImGuiPS.hlsl", "main", HShaderFrequency::SF_Pixel)

bool HImGuiPixelShader::ShouldCache(HShaderPlatform InShaderPlatform)
{
	return true;
}

void HImGuiPixelShader::ModifyCompilationEnvironment(HShaderPlatform InShaderPlatform, HShaderCompilerEnvironment& ShaderCompilerEnvironment)
{

}