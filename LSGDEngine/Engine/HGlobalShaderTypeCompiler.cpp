#include "HEnginePCH.h"
#include "HGlobalShaderTypeCompiler.h"

#include "HShader.h"
#include "HGlobalShader.h"
#include "HShaderCompilingManager.h"

#include "HShaderCompilerUtil.h"

using namespace lsgd;

HShaderCompileJob* HGlobalShaderTypeCompiler::BeginCompileShader(HGlobalShaderType* ShaderType, int32 PermutationId, HShaderPlatform Platform, const HShaderPipelineType* ShaderPipeline, HArray<HShaderCommonCompileJob*>& NewJobs)
{
	HShaderCompileJob* NewJob = new HShaderCompileJob(ShaderType);
	HShaderCompilerEnvironment& ShaderEnvironment = NewJob->Input.Environment;

	// allow the shader type to modify the compile environment
	ShaderType->SetupCompileEnvironment(Platform, ShaderEnvironment);

	static HString GlobalName("Global");

	HShaderTarget Target = { ShaderType->Frequency, Platform };

	// compile the shader environment passed with the shader type's source code
	HShaderCompilerUtil::GlobalBeginCompileShader(GlobalName, nullptr, ShaderType, ShaderPipeline,
		ShaderType->SourceFilename, ShaderType->FunctionName, Target, NewJob, NewJobs, true);

	return NewJob;
}