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

		// 
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
	};

	class HShader : public HDeferredCleanupInterface
	{
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

	};

}