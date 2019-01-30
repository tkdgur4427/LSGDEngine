#pragma once

namespace lsgd
{
	enum HShaderFrequency
	{
		SF_Vertex,
		SF_Pixel,
		SF_NumFrequency,
	};

	enum HShaderPlatform
	{
		SP_SM5,
		SP_NumPlatforms,
	};

	struct HShaderTarget
	{
		uint32 Frequency;
		uint32 Platform;
	};

	// the base type of a value in a uniform buffer
	enum HUniformBufferBaseType
	{
		UBT_INVALID,
		UBT_BOOL,
		UBT_INT32,
		UBT_UINT32,
		UBT_FLOAT32,
		UBT_STRUCT,
		UBT_SRV,
		UBT_SAMPLER,
		UBT_TEXTURE,
		UBT_Num,
		UBT_NumBits = 4,
	};

	// each entry in a resource table is provided to the shader compiler for creating mappings
	struct HResourceTableEntry
	{
		// the name of the uniform buffer in which this resource exists
		HString UniformBufferName;
		// the type of the resource (HUniformBufferBaseType)
		uint16 Type;
		// the index of the resource in the table
		uint16 ResourceIndex;
	};

	struct HShaderCompilerDefinitions
	{
		HHashMap<HString, HString> Definitions;
	};

	struct HCachedUniformBufferDeclaration
	{
		HString Declaration[HShaderPlatform::SP_NumPlatforms];
	};

	// a map of shader parameter names to registers allocated to that parameter
	class HShaderParameterMap
	{
	public:
		struct HParameterAllocation
		{
			uint16 BufferIndex;
			uint16 BaseIndex;
			uint16 Size;
			mutable bool bBound;
		};

		HHashMap<HString, HParameterAllocation> ParameterMap;
	};
}