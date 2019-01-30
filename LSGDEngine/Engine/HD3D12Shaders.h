#pragma once

#include "HRHIResource.h"
#include "HD3D12ShaderCommon.h"

namespace lsgd
{
	class HD3D12VertexShader : public HRHIVertexShader
	{
	public:
		HD3D12ShaderByteCode ShaderByteCode;
		HD3D12ShaderResourceTable ShaderResourceTable;

		// vertex shader's bytecode, with custom data in the last byte
		HArray<uint8> Code;

		HShaderCodePackedResourceCounts ResourceCounts;
	};

	class HD3D12PixelShader : public HRHIPixelShader
	{
	public:
		HD3D12ShaderByteCode ShaderByteCode;
		HD3D12ShaderResourceTable ShaderResourceTable;

		// vertex shader's bytecode, with custom data in the last byte
		HArray<uint8> Code;

		HShaderCodePackedResourceCounts ResourceCounts;
	};
}