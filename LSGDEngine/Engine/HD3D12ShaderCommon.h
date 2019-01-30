#pragma once

#include "HRHIShaderCommon.h"

namespace lsgd
{
	class HD3D12ShaderByteCode
	{
	public:
		D3D12_SHADER_BYTECODE Shader;
	};	

	struct HD3D12ShaderResourceTable : public HBaseShaderResourceTable
	{
		// mapping of bound textures to their location in resource tables
		HArray<uint32> TextureMap;
	};
}