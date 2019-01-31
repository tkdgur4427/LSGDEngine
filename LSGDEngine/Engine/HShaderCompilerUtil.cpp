#include "HEnginePCH.h"
#include "HShaderCompilerUtil.h"

#include "HShader.h"
#include "HVertexFactory.h"

using namespace lsgd;

void HShaderCompilerUtil::InitializeShaderTypes()
{
	HHashMap<HString, HArray<HString>> ShaderFileToUniformBufferVariables;
	BuildShaderFileToUniformBufferMap(ShaderFileToUniformBufferVariables);

	// @todo process with ShaderFileToUniformBufferVariables
	HShaderType::Initialize();
	HVertexFactoryType::Initialize();

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

void HShaderCompilerUtil::CompileGlobalShaderMap(HShaderPlatform Platform, bool bRefreshShaderMap)
{

}