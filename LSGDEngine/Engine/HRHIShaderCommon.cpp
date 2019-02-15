#include "HEnginePCH.h"
#include "HRHIShaderCommon.h"

using namespace lsgd;

bool BuildResourceTableMapping(const HHashMap<HString, HResourceTableEntry>& ResourceTableMap,
	HShaderParameterMap& ParameterMap, HShaderCompilerResourceTable& OutSRT)
{
	// build resource table mapping
	int32 MaxBoundResourceTable = -1;
	HArray<uint32> ResourceTableSRVs;
	HArray<uint32> ResourceTableSamplerStates;
	HArray<uint32> ResourceTableUAVs;

	// go through all the member of ALL the UB resources
	for ()
}