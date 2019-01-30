#pragma once

namespace lsgd
{
	// the environment used to compile a shader
	class HShaderCompilerEnvironment : public HRefCountedObject
	{
	public:
		HHashMap<HString, HString> IncludeFileNameToContentsMap;
		HArray<uint32> CompilerFlags;
		HHashMap<uint32, uint8> RenderTargetOutputFormatsMap;
		HHashMap<HString, HResourceTableEntry> ResourceTableMap;
		//HHashMap<HString, uint32> ResourceTableLayoutHashes;
		HShaderCompilerDefinitions Definitions;
	};
}