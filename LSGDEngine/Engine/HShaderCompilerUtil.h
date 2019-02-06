#pragma once

namespace lsgd
{
	struct HShaderCompilerInput;
	class HShaderCompilerOutput;

	// a collection of static method for clear classification
	class HShaderCompilerUtil
	{
	public:
		static void InitializeShaderTypes();
		static void BuildShaderFileToUniformBufferMap(HHashMap<HString, HArray<HString>>& ShaderFileToUniformBufferVariables);

		// generate a list of shader source files that engine needs to load
		static void GetAllShaderSourceFiles(HArray<HString>& ShaderSourceFiles);

		/*
			add a new entry to the list of shader source files
				- only unique entries can be loaded are added as well as their #include files
		*/
		static void AddShaderSourceFileEntry(HArray<HString>& ShaderSourceFiles, const HString& ShaderFilename);

		static void CompileGlobalShaderMap(HShaderPlatform Platform, bool bRefreshShaderMap);

		// make sure all global shaders are loaded and/or compiled for the passed in platform
		static void VerifyGlobalShaders(HShaderPlatform Platform, bool bLoadedFromCacheFile);

		// enqueues a shader compile job with GShaderCompilingManager
		static void GlobalBeginCompileShader(const HString& DebugGroupName, class HVertexFactoryType* VFType, class HShaderType* ShaderType,
			const class HShaderPipelineType* ShaderPipelineType, const HString& SourceFilename, const HString& FunctionName, HShaderTarget Target,
			class HShaderCompileJob* NewJob, HArray<class HShaderCommonCompileJob*>& NewJobs, bool bAllowDevelopmentShaderCompile);

		static void ProcessCompiledGlobalShaders(const HArray<class HShaderCommonCompileJob*>& CompilationResults);

		static class HShader* ProcessCompiledJob(class HShaderCompileJob* SingleJob, const class HShaderPipelineType* Pipeline, HArray<HShaderPlatform>& ShaderPlatformProcessed, HArray<const class HShaderPipelineType*>& OutSharedPipelines);

		static void ProcessCompilationJob(const HShaderCompilerInput& Input, HShaderCompilerOutput& Output, const HString& WorkingDirectory);
	};
}