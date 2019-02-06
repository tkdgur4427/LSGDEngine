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

	HShaderTarget Target(ShaderType->Frequency, Platform);

	// compile the shader environment passed with the shader type's source code
	HShaderCompilerUtil::GlobalBeginCompileShader(GlobalName, nullptr, ShaderType, ShaderPipeline,
		ShaderType->SourceFilename, ShaderType->FunctionName, Target, NewJob, NewJobs, true);

	return NewJob;
}

HShader* HGlobalShaderTypeCompiler::FinishCompileShader(HGlobalShaderType* ShaderType, const HShaderCompileJob& CurrentJob, const class HShaderPipelineType* ShaderPipelineType)
{
	HShader* Shader = nullptr;
	if (CurrentJob.bSuccessed)
	{
		HShaderType* SpecificType = CurrentJob.ShaderType; // um... LimitShaderResourceToThisType()

		// reuse an existing resource with the same key or create a new one based on the compiled output
		// this allows HShaders to share compiled bytecode and RHI shader resources
		HShaderResource* Resource = HShaderResource::FindOrCreateShaderResource(CurrentJob.Output, SpecificType);
		check(Resource);

		//... @todo processing pipeline

		// find a shader with the same key in memory
		Shader = CurrentJob.ShaderType->FindShaderById(HShaderId(CurrentJob.Input.Target, nullptr, ShaderType));

		// there was no shader with the same key so create a new one with the compile out, which will bind shader parameters
		if (!Shader)
		{
			HGlobalShaderType::CompiledShaderInitializerType Initializer(SpecificType, CurrentJob.Output, Resource, nullptr, nullptr);
			Shader = (*(ShaderType->ConstructCompiledRef))(Initializer);
		}
	}

	return Shader;
}