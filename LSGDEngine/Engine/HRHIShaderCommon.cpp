#include "HEnginePCH.h"
#include "HRHIShaderCommon.h"

using namespace lsgd;

bool HBuildResourceTableMapping(const HHashMap<HString, HResourceTableEntry>& ResourceTableMap,
	HShaderParameterMap& ParameterMap, HShaderCompilerResourceTable& OutSRT)
{
	// build resource table mapping
	int32 MaxBoundResourceTable = -1;
	HArray<uint32> ResourceTableSRVs;
	HArray<uint32> ResourceTableSamplerStates;
	HArray<uint32> ResourceTableUAVs;

	// go through all the member of ALL the UB(Uniform Buffer) resources
	for (auto& ResourceTable : ResourceTableMap)
	{
		const HString& Name = ResourceTable.first;
		const HResourceTableEntry& ResourceTableEntry = ResourceTable.second;

		
	}

	return true;
}