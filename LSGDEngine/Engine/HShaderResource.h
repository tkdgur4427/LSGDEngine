#pragma once

#include "HRenderResource.h"
#include "HDeferredCleanupInterface.h"

namespace lsgd {

	/*
		uniquely identifies an HShaderResource
		used to link HShaders to HShaderResources on load
	*/
	class HShaderResourceId
	{
	public:
		HShaderResourceId(const HString ShaderTypename);
		~HShaderResourceId();

		friend inline uint32 GetTypeHash(const HShaderResourceId& Id)
		{
			// @todo override...
			return 0;
		}

		friend bool operator==(const HShaderResourceId& A, const HShaderResourceId& B)
		{
			return (A.Target == B.Target)
				&& (A.SpecificShaderTypeStorage == B.SpecificShaderTypeStorage);
		}

		friend bool operator!=(const HShaderResourceId& A, const HShaderResourceId& B)
		{
			return !(A == B);
		}

		// target platform and frequency
		HShaderTarget Target;

		// hash of the compiled shader output which is used to create the HShaderResource
		// FSHAHash OutputHash;

		// null if type doesn't matter, otherwise the name of the type that this was created specifically for, which is used with geometry shader stream out
		const char* SpecificShaderTypename;

		// stores memory for specificshadertypename; if this is a standalone Id, otherwise is empty and specificshadertypename points to an HShaderType name
		HString SpecificShaderTypeStorage;
	};
}

USE_HASH_OVERRIDE(lsgd::HShaderResourceId)

namespace lsgd {

	// compiled shader bytecode and its corresponding RHI resource; this can be shared by multiple FShader with identical compiled output
	class HShaderResource : public HRenderResource, public HDeferredCleanupInterface
	{
	public:
		HShaderResource(const class HShaderCompilerOutput& Output, class HShaderType* InSpecificType, int32 InSpecificPermutationId);
		virtual ~HShaderResource();

		HShaderResourceId GetId() const;

		void CompressCode(const HArray<uint8>& UncompressedCode);
		void UncompressCode(HArray<uint8>& UncompressCode);

		// to be used in HRefCountPtr
		uint32 AddRef() const
		{
			return uint32(++NumRefs);
		}

		uint32 Release() const
		{
			uint32 Refs = uint32(--NumRefs);
			if (Refs == 0)
			{
				delete this;
			}
			return Refs;
		}

		uint32 GetRefCount() const
		{
			return uint32(NumRefs);
		}

		virtual void InitRHI() override;
		virtual void FinishCleanup() override {}

		static HShaderResource* FindShaderResourceById(const HShaderResourceId& Id);
		static HShaderResource* FindOrCreateShaderResource(const class HShaderCompilerOutput& Output, class HShaderType* SpecificType);

		// reference to the RHI shader; only one of these is ever valid
		HRefCountPtr<class HRHIVertexShader> VertexShader;
		HRefCountPtr<class HRHIPixelShader> PixelShader;

		HShaderTarget ShaderTarget;

		// compiled byte code
		HArray<uint8> Code;

		// original bytecode size before compression
		uint32 UncompressedCodeSize;

		// hash of the compiled bytecode and the generated parametermap
		// FSHAHash OutputHash;

		// if not null, the shader-type this shader must be used with
		class HShaderType* SpecificType;

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
		static HHashMap<HShaderResourceId, HShaderResource*> ShaderResourceIdMap;
		static HCriticalSection ShaderResourceIdMapCritical;
	};

}
