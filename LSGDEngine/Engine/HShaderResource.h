#pragma once

#include "HRenderResource.h"
#include "HDeferredCleanupInterface.h"

namespace lsgd {

	// compiled shader bytecode and its corresponding RHI resource; this can be shared by multiple FShader with identical compiled output
	class HShaderResource : public HRenderResource, public HDeferredCleanupInterface
	{
	public:
		HShaderResource();
		virtual ~HShaderResource();

		// reference to the RHI shader; only one of these is ever valid
		HRefCountPtr<HRHIVertexShader> VertexShader;
		HRefCountPtr<HRHIPixelShader> PixelShader;

		HShaderTarget ShaderTarget;

		// compiled byte code
		HArray<uint8> Code;

		// original bytecode size before compression
		uint32 UncompressedCodeSize;

		// hash of the compiled bytecode and the generated parametermap
		// FSHAHash OutputHash;

		// if not NULL, the shader-type this shader must be used with
		uint32 NumInstructions;

		// number of texture samplers the shader uses
		uint32 NumTextureSamplers;

		// number of references to this shader
		mutable uint32 NumRefs;

		// a canary used to detect when a stale shader resource is being rendered with
		//	- ShaderMagic_Uninitialized, ShaderMagic_CleaningUp, ShaderMagic_Initialized in FShader
		uint32 Canery;

		// whether the shdaer code is stored in a shader library
		//bool bCodeInSharedLocation;

		// tracks loaded shader resource by id
		//static HHashMap<HShaderResourceId, HShaderResource*> ShaderResourceIdMap;
		//static HCriticalSection ShaderResourceIdMapCritical;
	};

}
