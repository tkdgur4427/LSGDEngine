#pragma once

namespace lsgd {
	// uniquely identifies an HShader; used to link HMaterialShaderMaps and HShader on load
	class HShaderId
	{
	public:
		HShaderId(const HShaderTarget& InShaderTarget, class HVertexFactoryType* InVertexFactoryType, class HShaderType* InShaderType);
		~HShaderId();

		friend inline uint32 GetTypeHash(const HShaderId& Id)
		{
			// @todo override...
			return 0;
		}

		friend bool operator==(const HShaderId& A, const HShaderId& B)
		{
			return (A.ShaderPipeline == B.ShaderPipeline)
				&& (A.VertexFactoryType == B.VertexFactoryType)
				&& (A.ShaderType == B.ShaderType)
				&& (A.Target == B.Target);
		}

		friend bool operator!=(const HShaderId& A, const HShaderId& B)
		{
			return !(A == B);
		}

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

		// shader platform and frequency
		HShaderTarget Target;
	};

	// define a shader permutation uniquely according to its type, and permutation id
	template <typename MetaShaderType>
	struct HShaderTypePermutation
	{
		HShaderTypePermutation() {}
		~HShaderTypePermutation() {}

		friend inline uint32 GetTypeHash(const HShaderTypePermutation& Id)
		{
			// @todo override...
			return 0;
		}

		friend bool operator==(const HShaderTypePermutation& A, const HShaderTypePermutation& B)
		{
			return (A.Type == B.Type) && (A.PermutationId == B.PermutationId);
		}

		friend bool operator!=(const HShaderTypePermutation& A, const HShaderTypePermutation& B)
		{
			return !(A == B);
		}

		MetaShaderType* const Type;
		const int32 PermutationId;
	};
}

// override std::hash
USE_HASH_OVERRIDE(lsgd::HShaderId)