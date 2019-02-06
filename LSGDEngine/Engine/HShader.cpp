#include "HEnginePCH.h"
#include "HShader.h"

// global shader
#include "HGlobalShader.h"

#include "HVertexFactory.h"

#include "HShaderResource.h"

#include "HShaderCompilingManager.h"

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

HShader* HShaderType::FindShaderById(const HShaderId& Id)
{
	auto ResultIter = ShaderIdMap.find(Id);
	if (ResultIter != ShaderIdMap.end())
	{
		return ResultIter->second;
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

lsgd::HShader::CompiledShaderInitializerType::CompiledShaderInitializerType(
	HShaderType* InShaderType,
	const HShaderCompilerOutput& InOutput,
	HShaderResource* InResource,
	HShaderPipelineType* InShaderPipelineType,
	HVertexFactoryType* InVertexFactoryType)
	: Type(InShaderType)
	, Target(InOutput.Target)
	, Resource(InResource)
	, ShaderPipeline(InShaderPipelineType)
	, VertexFactoryType(InVertexFactoryType)
	, ParameterMap(InOutput.ParameterMap)
	, Code(InOutput.ShaderCode.ShaderCodeWithOptionalData)
{

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
