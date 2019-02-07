#pragma once

#include "..\Core\HCore.h"

// d3d12 - temporary (to include dx math)
//	@todo - need to change this after implementing direct x 12 layer
#include "d3d12.h"

#include "D3DCompiler.h"

#include "HMath.h"

#include "HRenderingThread.h"

#include "HRHICommon.h"

#include "HRefCountPtr.h"

// to use guid in dx11; e.g. IID_ID3D11ShaderReflection
#pragma comment(lib, "dxguid.lib")
