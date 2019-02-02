#pragma once

namespace lsgd
{
	class HGlobalShaderTypeCompiler
	{
	public:
		static class HShaderCompileJob* BeginCompileShader(class HGlobalShaderType* ShaderType, int32 PermutationId, HShaderPlatform Platform, const class HShaderPipelineType* ShaderPipeline, HArray<class HShaderCommonCompileJob*>& NewJobs);
	};
}