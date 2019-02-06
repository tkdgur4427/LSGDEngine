#include "HEnginePCH.h"
#include "HShaderResource.h"
#include "HRHIResource.h"

#include "HShader.h"
#include "HShaderCompilingManager.h"

using namespace lsgd;

HShaderResourceId::HShaderResourceId(const HString ShaderTypename)
	: SpecificShaderTypeStorage(ShaderTypename)
{
	SpecificShaderTypename = &SpecificShaderTypeStorage[0];
}

HShaderResourceId::~HShaderResourceId()
{

}

HHashMap<HShaderResourceId, HShaderResource*> HShaderResource::ShaderResourceIdMap;
HCriticalSection HShaderResource::ShaderResourceIdMapCritical;

HShaderResource::HShaderResource(const HShaderCompilerOutput& Output, HShaderType* InSpecificType, int32 InSpecificPermutationId)
	: NumRefs(0)
	, SpecificType(InSpecificType)
	, NumInstructions(Output.NumInstructions)
	, NumTextureSamplers(Output.NumTextureSamplers)
	//, Canary(HShader::ShaderMagic_Initialized)
{
	ShaderTarget = Output.Target;
	CompressCode(Output.ShaderCode.ShaderCodeWithOptionalData);

	ShaderResourceIdMap.insert({ GetId(), this });
}

HShaderResource::~HShaderResource()
{
	check(NumRefs == 0);

	ShaderResourceIdMap.erase(GetId());
}

HShaderResourceId HShaderResource::GetId() const
{
	return HShaderResourceId(SpecificType->Name);
}

void HShaderResource::CompressCode(const HArray<uint8>& UncompressedCode)
{
	// @todo - compress/uncompress code
	UncompressedCodeSize = UncompressedCode.size();
	Code = UncompressedCode;
}

HShaderResource* HShaderResource::FindShaderResourceById(const HShaderResourceId& Id)
{
	auto ResultIter = ShaderResourceIdMap.find(Id);
	if (ResultIter != ShaderResourceIdMap.end())
	{
		return ResultIter->second;
	}
	return nullptr;
}

HShaderResource* HShaderResource::FindOrCreateShaderResource(const HShaderCompilerOutput& Output, HShaderType* SpecificType)
{
	const HShaderResourceId ResourceId(SpecificType ? SpecificType->Name : "");
	HShaderResource* Resource = FindShaderResourceById(ResourceId);

	if (!Resource)
	{
		Resource = new HShaderResource(Output, SpecificType, 0/*temporary... @todo*/);
	}
	else
	{
		//check(Resource->Canary == HShader::ShaderMagic_Initialized);
	}
	return Resource;
}