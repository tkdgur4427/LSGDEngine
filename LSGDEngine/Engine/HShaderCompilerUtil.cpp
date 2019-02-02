#include "HEnginePCH.h"
#include "HShaderCompilerUtil.h"

#include "HShader.h"
#include "HVertexFactory.h"

// global shader
#include "HGlobalShader.h"

// shader compiler
#include "HShaderCompilingManager.h"

// global shader compiler
#include "HGlobalShaderTypeCompiler.h"

using namespace lsgd;

void HShaderCompilerUtil::InitializeShaderTypes()
{
	HHashMap<HString, HArray<HString>> ShaderFileToUniformBufferVariables;
	BuildShaderFileToUniformBufferMap(ShaderFileToUniformBufferVariables);

	// @todo process with ShaderFileToUniformBufferVariables
	HShaderType::Initialize();
	//HVertexFactoryType::Initialize();

	// HShaderPipelineType::Initialize();
}

void HShaderCompilerUtil::BuildShaderFileToUniformBufferMap(HHashMap<HString, HArray<HString>>& ShaderFileToUniformBufferVariables)
{
	HArray<HString> ShaderSourceFiles;
	GetAllShaderSourceFiles(ShaderSourceFiles);

	// cache UB.. @todo
}

void HShaderCompilerUtil::AddShaderSourceFileEntry(HArray<HString>& ShaderSourceFiles, const HString& ShaderFilename)
{
	// get all shader resource files
	HString ShaderFilenameBase(HGenericPlatformMisc::GetShaderSourceDir() + ShaderFilename);

	// if already processed shader source file, skip
	for (auto& ShaderSourceFile : ShaderSourceFiles)
	{
		if (ShaderSourceFile == ShaderFilename)
		{
			return;
		}
	}

	// get the filename for the VF type
	ShaderSourceFiles.push_back(ShaderFilenameBase);

	HArray<HString> ShaderIncludes;
	// GetShaderIncludes(ShaderFilenameBase, ShaderIncludes);
	for (auto& ShaderInclude : ShaderIncludes)
	{
		ShaderSourceFiles.push_back(ShaderInclude);
	}
}

void HShaderCompilerUtil::GetAllShaderSourceFiles(HArray<HString>& ShaderSourceFiles)
{
	// *** caching all shader source files to ShaderSourceFiles

	// looping vertex factory types
	HList<HVertexFactoryType*>& VertexFactoryTypes = *(HVertexFactoryType::GetList());
	for (auto& VertexFactoryType : VertexFactoryTypes)
	{
		if (VertexFactoryType)
		{
			HString ShaderFilename = VertexFactoryType->SourceFilename;
			AddShaderSourceFileEntry(ShaderSourceFiles, ShaderFilename);
		}
	}

	// looping shader types
	HList<HShaderType*>& ShaderTypes = *(HShaderType::GetList());
	for (auto& ShaderType : ShaderTypes)
	{
		if (ShaderType)
		{
			HString ShaderFilename = ShaderType->SourceFilename;
			AddShaderSourceFileEntry(ShaderSourceFiles, ShaderFilename);
		}
	}

	// basic common types (e.g. MaterialTemplate.usf)
	//...
}

void HShaderCompilerUtil::GlobalBeginCompileShader(const HString& DebugGroupName, HVertexFactoryType* VFType, HShaderType* ShaderType,
	const HShaderPipelineType* ShaderPipelineType, const HString& SourceFilename, const HString& FunctionName, HShaderTarget Target,
	class HShaderCompileJob* NewJob, HArray<HShaderCommonCompileJob*>& NewJobs, bool bAllowDevelopmentShaderCompile)
{
	HShaderCompilerInput& Input = NewJob->Input;
	Input.Target = Target;
	Input.ShaderFormat = "SM5"; // @todo - temporary, need to convert Target.Platform appropriately
	Input.SourceFilename = SourceFilename;
	Input.EntryPointName = FunctionName;
	Input.bCompilingForShaderPipeline = false;
	Input.bIncludeUsedOutputs = false;
	Input.bGenerateDirectCompileFile = false; // @todo need to handle shader debug info process
	//Input.DumpDebugInfoPath = ...;
	Input.DebugGroupName = DebugGroupName;

	// add the appropriate definitions for the shader frequency
	{
		Input.Environment.SetDefine("VERTEXSHADER", Target.Frequency == HShaderFrequency::SF_Vertex);
		Input.Environment.SetDefine("PIXELSHADER", Target.Frequency == HShaderFrequency::SF_Pixel);
	}

	//... additional SetDefine (define macro insertions)

	// UB processing
	// ShaderType->AddReferencedUniformBufferIncludes(...);
	// if (VFType) { VFType->AddReferencedUniformBufferIncludes(...); }

	NewJobs.push_back(NewJob);
}

void HShaderCompilerUtil::VerifyGlobalShaders(HShaderPlatform Platform, bool bLoadedFromCacheFile)
{
	// all jobs, single & pipeline
	HArray<HShaderCommonCompileJob*> GlobalShaderJobs;

	// add the single jobs first
	HHashMap<HShaderTypePermutation<const HShaderType>, HShaderCompileJob*> SharedShaderJobs;

	HList<HShaderType*>& ShaderTypes = *(HShaderType::GetList());
	for (auto& ShaderType : ShaderTypes)
	{
		HGlobalShaderType* GlobalShaderType = ShaderType->GetGlobalShaderType();
		if (!GlobalShaderType)
		{
			continue;
		}

		// @todo checking with permutation id.. (need to understand what shader permutation meaning about)

		// compile this global shader type
		auto* Job = HGlobalShaderTypeCompiler::BeginCompileShader(GlobalShaderType, 0, Platform, nullptr, GlobalShaderJobs);
	}
}

void HShaderCompilerUtil::CompileGlobalShaderMap(HShaderPlatform Platform, bool bRefreshShaderMap)
{
	if (!GGlobalShaderMap[Platform].get())
	{
		// cache GGShaderFileCache
		// VerifyShaderSourceFiles();

		// create new global shader map
		GGlobalShaderMap[Platform] = make_unique<HShaderMap<HGlobalShaderType>>(Platform);

		bool bLoadedFromCacheFile = false;

		{
			HGlobalShaderMapId ShaderMapId(Platform);

			// @todo - cache the global shader compilation data in binary
		}

		// if any shaders weren't loaded, compile them now
		VerifyGlobalShaders(Platform, bLoadedFromCacheFile);
	}
}