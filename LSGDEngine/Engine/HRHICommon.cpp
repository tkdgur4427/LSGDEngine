#include "HEnginePCH.h"
#include "HRHICommon.h"

using namespace lsgd;

void HShaderParameterMap::AddParameterAllocation(const HString& InParameterName,
	uint16 InBufferIndex, uint16 InBaseIndex, uint16 InSize)
{
	HParameterAllocation NewParameter;
	NewParameter.BufferIndex = InBufferIndex;
	NewParameter.BaseIndex = InBaseIndex;
	NewParameter.Size = InSize;

	ParameterMap.insert({ InParameterName, NewParameter });
}