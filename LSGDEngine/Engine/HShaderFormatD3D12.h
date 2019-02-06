#pragma once

#include "HShaderFormat.h"

namespace lsgd {

	class HShaderFormatD3D12 : public HShaderFormat
	{
	public:
		virtual void CompileShader(const class HShaderCompilerInput& Input, class HShaderCompilerOutput& Output, const HString& WorkingDirectory) override;
	};

}