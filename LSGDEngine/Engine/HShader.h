#pragma once

#include "HDeferredCleanupInterface.h"
#include "HShaderResource.h"

namespace lsgd {

	// uniquely identifies an HShader; used to link HMaterialShaderMaps and HShader on load
	class HShaderId
	{
	public:
		/*
			hash of the material shader map id, since this shader depends on the generated material code from that shader map
			a hash is used instead of the full shader map id to shorten the key length, even though this will result in a hash being hashed
		*/
		// FSHAHash MaterialShaderMapHash;

		// shader pipeline linked to this shader, needed since a single might be used on different pipelines
		const class HShaderPipelineType* ShaderPipeline;

		// vertex factory type that the shader was created for, this is needed in the id since a single shader type will be compiled for multiple vertex factories within a material shader map will be NULL for global shaders
		class HVertexFactoryType* VertexFactoryType;

		// used to detect changes to the VF source files
		// FSHAHash VFSourceHash;

		// shader type
		class HShaderType* ShaderType;

		// used to detect changes to the shader source files
		// FSHAHash SourceHash;

		// shader platform and freqeuncy
		HShaderTarget Target;
	};

	// an object which is used to serialize/deserialize, compile, and cache a particular shader class
	class HShaderType
	{
	public:
		enum class HShaderTypeForDynamicCast : uint32
		{
			Global,
			Material,
			MeshMaterial,
		};

		typedef class HShader* (*ConstructSerializedType)();
		
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

		HShader();
		virtual ~HShader();		

		virtual void FinishCleanup();

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
}

// a macro to declare a new shader type; this should be called in the class body of the new shader type
#define DECLARE_EXPORTED_SHADER_TYPE(ShaderClass, ShaderMetaTypeShortcut) \
	public: \
		typedef lsgd::H##ShaderMetaTypeShortcut##ShaderClass ShaderMetaType; \
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
		FunctionName, \
		Frequency, \
		lsgd::ShaderClass::ConstructSerializedInstance, \
		lsgd::ShaderClass::ConstructCompiledInstance, \
		lsgd::ShaderClass::ModifyCompilationEnvironment, \
		lsgd::ShaderClass::ShouldCache, \
		);
		