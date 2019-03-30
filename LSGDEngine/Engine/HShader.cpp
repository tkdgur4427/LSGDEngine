#include "HEnginePCH.h"
#include "HShader.h"

// global shader
#include "HGlobalShader.h"

#include "HVertexFactory.h"

#include "HShaderResource.h"

#include "HShaderCompilingManager.h"

using namespace lsgd;

HShaderType::HShaderType(HShaderTypeForDynamicCast InShaderTypeForDynamicCast, const HString& InSourceFilename,
	const HString& InFunctionName, int32 InFrequency, ConstructSerializedType ConstructSerialized)
	: ShaderTypeForDynamicCast(InShaderTypeForDynamicCast)
	, ConstructSerializedRef(ConstructSerialized)
	, SourceFilename(InSourceFilename)
	, FunctionName(InFunctionName)
	, Frequency(InFrequency)
{
	//@todo - temporary setting name as combination with source file and function name
	Name = (SourceFilename + FunctionName);

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

HShader::HShader(const CompiledShaderInitializerType& InInitializer)
	: NumRefs(0)
	, SerializedResource(InInitializer.Resource)
	, Resource(InInitializer.Resource)
	, Type(InInitializer.Type)
	, Target(InInitializer.Target)
	, Canary(ShaderMagic_Uninitialized)
{

}

HShader::~HShader()
{
	check(NumRefs == 0);
}

void HShader::FinishCleanup()
{

}

void HShader::BeginInitializeResource()
{
	BeginInitResource(Resource);
}
