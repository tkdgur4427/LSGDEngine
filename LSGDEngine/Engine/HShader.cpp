#include "HEnginePCH.h"
#include "HShader.h"

// global shader
#include "HGlobalShader.h"

using namespace lsgd;

HShaderType::HShaderType(HShaderTypeForDynamicCast InShaderTypeForDynamicCast)
	: ShaderTypeForDynamicCast(InShaderTypeForDynamicCast)
{
	AddToGlobalList(this);
}

HShaderType::~HShaderType()
{
	RemvoeFromGlobalList(this);
}

HGlobalShaderType* HShaderType::GetGlobalShaderType()
{
	if (ShaderTypeForDynamicCast == HShaderType::HShaderTypeForDynamicCast::Global)
	{
		return (HGlobalShaderType*)this;
	}
	return nullptr;
}

void HShaderType::Initialize()
{
	HList<HShaderType*>& ShaderTypes = *(GetList());
	for (auto& ShaderType : ShaderTypes)
	{
		// GenerateReferencedUniformBuffer(...); - @todo
		// ...
	}
}

HShader::HShader(const CompiledShaderInitializerType&)
	: NumRefs(0)
{

}

HShader::~HShader()
{
	check(NumRefs == 0);
}

void HShader::FinishCleanup()
{

}
