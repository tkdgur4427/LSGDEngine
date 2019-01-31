#include "HEnginePCH.h"
#include "HShader.h"

using namespace lsgd;

HShaderType::HShaderType()
{
	AddToGlobalList(this);
}

HShaderType::~HShaderType()
{
	RemvoeFromGlobalList(this);
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

HShader::HShader()
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
