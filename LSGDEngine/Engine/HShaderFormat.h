#pragma once

namespace lsgd
{
	class HShaderFormat
	{
	public:
		virtual void CompileShader(const class HShaderCompilerInput& Input, class HShaderCompilerOutput& Output, const HString& WorkingDirectory) = 0;

		static HShaderFormat* FindOrCreateShaderFormat(const HString& ShaderFormat);
	};
}