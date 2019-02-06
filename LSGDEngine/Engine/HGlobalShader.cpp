#include "HEnginePCH.h"
#include "HGlobalShader.h"

using namespace lsgd;

unique_ptr<HShaderMap<HGlobalShaderType>> GGlobalShaderMap[HShaderPlatform::SP_NumPlatforms];

HGlobalShaderMapId::HGlobalShaderMapId(HShaderPlatform InPlatform)
{
	// @todo construct the dependencies informations
}

HGlobalShaderType::HGlobalShaderType(const HString& InSourceFilename, const HString& InFunctionName, int32 InFrequency, ConstructSerializedType ConstructSerialized, ConstructCompiledType ConstructCompiled, ShouldCacheType ShouldCache, ModifyCompilationEnvironmentType ModifyCompilationEnvironment)
	: HShaderType(HShaderTypeForDynamicCast::Global, InSourceFilename, InFunctionName, InFrequency, ConstructSerialized)
	, ConstructCompiledRef(ConstructCompiled)
	, ShouldCacheRef(ShouldCache)
	, ModifyCompilationEnvironmentRef(ModifyCompilationEnvironment)
{

}

HGlobalShaderType::~HGlobalShaderType()
{

}