#pragma once

namespace lsgd
{
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
	};
}