#include "HEnginePCH.h"
#include "HGlobalShader.h"

using namespace lsgd;

unique_ptr<HShaderMap<HGlobalShaderType>> GGlobalShaderMap[HShaderPlatform::SP_NumPlatforms];

HGlobalShaderMapId::HGlobalShaderMapId(HShaderPlatform InPlatform)
{
	// @todo construct the dependencies informations
}

HGlobalShaderType::HGlobalShaderType(HShaderTypeForDynamicCast InShaderTypeForDynamicCast)
	: HShaderType(InShaderTypeForDynamicCast)
{

}

HGlobalShaderType::~HGlobalShaderType()
{

}