#pragma once

namespace lsgd
{
	struct HBaseShaderResourceTable
	{
		// bits indicating which resource tables contain resources bound to this shader
		uint32 ResourceTableBits;

		// mapping of bound SRVs to their location in resource table
		HArray<uint32> ShaderResourceViewMap;

		// mapping of bound sampler states to their location in resource table
		HArray<uint32> SamplerMap;

		// mapping of bound UAVs
		HArray<uint32> UnorderedAccessViewMap;

		// hash of the layouts of resource tables at compile time, used for runtime validation
		//HArray<uint32> ResourceTableLayoutHashes;
	};

	// if this changes, you need to make sure all rhi shaders get invalidate
	struct HShaderCodePackedResourceCounts
	{
		bool bGlobalUniformBufferUsed;
		uint8 NumSamplers;
		uint8 NumSRVs;
		uint8 NumCBs;
		uint8 NumUAVs;
	};
}