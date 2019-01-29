#pragma once

#include "HDeferredCleanupInterface.h"

namespace lsgd {

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
		//HShaderResource*
	};

}