#pragma once

#include "HDeferredCleanupInterface.h"
#include "HShaderResource.h"
#include "HShaderId.h"

namespace lsgd {

	// an object which is used to serialize/deserialize, compile, and cache a particular shader class
	class HShaderType : public HGlobalLinkedList<HShaderType>
	{
	public:
		enum class HShaderTypeForDynamicCast : uint32
		{
			Global,
			Material,
			MeshMaterial,
		};

		typedef class HShader* (*ConstructSerializedType)();

		HShaderType(HShaderTypeForDynamicCast InShaderTypeForDynamicCast, const HString& InSourceFilename, 
			const HString& InFunctionName, int32 InFrequency, ConstructSerializedType ConstructSerialized);
		virtual ~HShaderType();

		class HGlobalShaderType* GetGlobalShaderType();

		class HShader* FindShaderById(const HShaderId& Id);

		static void Initialize();
		
		HShaderTypeForDynamicCast ShaderTypeForDynamicCast;
		uint32 HashIndex;
		HString Name;
		HString SourceFilename;
		HString FunctionName;
		uint32 Frequency;

		ConstructSerializedType ConstructSerializedRef;

		// a map from shader id to shader; a shader will be removed from it when deleted, so this doesn't need to use a HRefCountPtr
		HHashMap<HShaderId, class HShader*> ShaderIdMap;

		/*
			cache of referenced uniform buffer includes
			these are derived from source files so they need to be flushed when editing and recompiling shaders on the fly
		*/
		HHashMap<HString, HCachedUniformBufferDeclaration> ReferencedUniformBufferStructsCache;

		// tracks what platforms ReferencedBufferStructsCache has had declarations cached for
		bool bCachedUniformBufferStructDeclarations[HShaderPlatform::SP_NumPlatforms];
	};

	class HShader : public HDeferredCleanupInterface
	{
	public:
		struct CompiledShaderInitializerType
		{
			// temporary
			CompiledShaderInitializerType()
				: Code(HArray<uint8>())
				, ParameterMap(HShaderParameterMap())
			{}

			CompiledShaderInitializerType(
				class HShaderType* InShaderType,
				const class HShaderCompilerOutput& InOutput,
				class HShaderResource* InResource,
				class HShaderPipelineType* InShaderPipelineType,
				class HVertexFactoryType* InVertexFactoryType);

			HShaderType* Type;
			HShaderTarget Target;
			const HArray<uint8>& Code;
			const HShaderParameterMap& ParameterMap;
			// const FSHAHash& OutputHash;
			HShaderResource* Resource;
			// FSHAHash MaterialShaderMapHash;
			const class HShaderPipelineType* ShaderPipeline;
			class HVertexFactoryType* VertexFactoryType;
		};

		HShader(const CompiledShaderInitializerType&);
		virtual ~HShader();		

		virtual void FinishCleanup();

		virtual uint32 GetTypeSize() const { return sizeof(this); }

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

		// indexed the same as UniformBufferParameter; packed densely for coherent traversal
		// TArray<FUniformBufferStruct*> UniformBufferParameterStructs;
		// TArray<FShaderUniformBufferParameter*> UniformBufferParameters;

		// hash of the compiled output from this shader and the resulting parameter map this is used to find a matching resource
		// FSHAHash OutputHash;

		// pointer to the shader resource that has been serialized from disk, to be registered on the main thread later
		HShaderResource* SerializedResource;

		// reference to the shader resource, which stores the compiled bytecode and the RHI shader resource
		HRefCountPtr<HShaderResource> Resource;

		// hash of the material shader map this shader belongs to, stored so that an FShaderId can be constructed from this shader
		// FSHAHash MaterialShaderMapHash;

		// shader pipeline this shader belongs to, stored so that an HShaderID can be constructed from this shader
		const class HShaderPipelineType* ShaderPipeline;

		// vertex factory type this shader was created for, stored so that an HShaderId can be constructed
		// FSHAHash VFSourceHash;

		// shader type metadata for this shader
		class HShaderType* Type;

		// hash of this shader's source files generated at compile time, and stored to allow creating an FShaderId
		// FSHAHash SourceHash;

		HShaderTarget Target;

		mutable uint32 NumRefs;

		// transient value used to track when this shader's automatically bound uniform buffer parameters were set last
		mutable uint32 SetParameterId;

		// a canary used to detect when a stale shader is being rendered with
		uint32 Canary;

		// canary is set to this if the HShader is a valid pointer but uninitialized
		static const uint32 ShaderMagic_Uninitialized = 0xbd9922df;
		// canary is set to this if the FShader is a valid pointer but in the process of being cleaned up
		static const uint32 ShaderMagic_CleaningUp = 0xdc67f93b;
		// canary is set to this if the FShader is a valid pointer and initialized
		static const uint32 ShaderMagic_Initialized = 0x335b43ab;
	};

	// a collection of shaders of different types, but the same meta type
	template <typename ShaderMetaType>
	class HShaderMap
	{
	public:
		HShaderMap(HShaderPlatform InPlatform)
			: Platform(InPlatform)
		{}

		virtual ~HShaderMap() {}

		// container for serialized shader pipeline stages to be registered on the game thread
		struct HSerializedShaderPipeline
		{
			const class HShaderPipelineType* ShaderPipelineType;
			HArray<HRefCountPtr<HShader>> ShaderStages;
		};

		void AddShader(class HShaderType* ShaderType, class HShader* InShader);

		// list of serialized shaders to be processed and registered on the game thread
		HArray<HShader*> SerializedShaders;
		// list of serialized shader pipeline stages to be processed and registered on the game thread
		HArray<HSerializedShaderPipeline*> SerializedShaderPipelines;

		HShaderPlatform Platform;

		// flag that make sure this shader isn't used until all shader have been registered
		bool bHasBeeenRegistered;

		HHashMap<HShaderType*, HRefCountPtr<HShader>> Shaders;
		HHashMap<const class HShaderPipelineType*, class HShaderPipeline*> ShaderPipelines;
	};

	// encapsulate a dependency on shader type and saved state from that shader type
	class HShaderTypeDependency
	{
	public:
		// shader type
		HShaderType* ShaderType;

		// unique permutation identifier of the global shader type
		int32 PermutationId;

		// used to detect changes to the shader source files
		// FSHAHash StageSourceHash;
	};

	class HShaderPipelineTypeDependency
	{
	public:
		const class HShaderPipelineType* ShaderPipelineType;

		// FSHAHash StageSourceHash;
	};

	// template class implementations

	template <typename ShaderMetaType>
	void HShaderMap<ShaderMetaType>::AddShader(HShaderType* ShaderType, HShader* InShader)
	{
		auto ResultIter = Shaders.find(ShaderType);
		if (ResultIter == Shaders.end())
		{
			Shaders.insert({ ShaderType, InShader });
		}
	}
}

// hacky hacky... need to fix later @todo
USE_HASH_OVERRIDE(lsgd::HShaderTypePermutation<const lsgd::HShaderType>)

// a macro to declare a new shader type; this should be called in the class body of the new shader type
#define DECLARE_EXPORTED_SHADER_TYPE(ShaderClass, ShaderMetaTypeShortcut) \
	public: \
		typedef lsgd::H##ShaderMetaTypeShortcut##ShaderType ShaderMetaType; \
		static ShaderMetaType StaticType; \
		static lsgd::HShader* ConstructSerializedInstance() { return new ShaderClass; } \
		static lsgd::HShader* ConstructCompiledInstance(const ShaderMetaType::CompiledShaderInitializerType& Initializer) \
		{ return new lsgd::ShaderClass(Initializer); } \
		virtual uint32 GetTypeSize() const override { return sizeof(*this); }

#define DECLARE_SHADER_TYPE(ShaderClass, ShaderMetaTypeShortcut) \
	DECLARE_EXPORTED_SHADER_TYPE(ShaderClass, ShaderMetaTypeShortcut)

#define IMPLEMENT_SHADER_TYPE(TemplatePrefix, ShaderClass, SourceFilename, FunctionName, Frequency) \
	TemplatePrefix \
	lsgd::ShaderClass::ShaderMetaType lsgd::ShaderClass::StaticType( \
		#SourceFilename, \
		#FunctionName, \
		Frequency, \
		&lsgd::ShaderClass::ConstructSerializedInstance, \
		&lsgd::ShaderClass::ConstructCompiledInstance, \
		&lsgd::ShaderClass::ShouldCache, \
		&lsgd::ShaderClass::ModifyCompilationEnvironment \
		);
		