#pragma once

#include "HRenderResource.h"
#include "HDeferredCleanupInterface.h"

namespace lsgd {

	// compiled shader bytecode and its corresponding RHI resource; this can be shared by multiple FShader with identical compiled output
	class HShaderResource : public HRenderResource, public HDeferredCleanupInterface
	{
	public:

	};

}
