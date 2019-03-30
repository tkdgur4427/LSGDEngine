#pragma once

#include "..\Core\HCore.h"

#include "Windows.h"

// @todo - temp setting for directx 12
#define USE_DX12_AS_DEFAULT 1

#if USE_DX12_AS_DEFAULT
// d3d12 - temporary (to include dx math)
//	@todo - need to change this after implementing direct x 12 layer
#include <d3d12.h>
#include <dxgi1_4.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#endif

#include "D3DCompiler.h"

#include "HMath.h"

#include "HRenderingThread.h"

#include "HRHICommon.h"

#include "HRefCountPtr.h"

// to use guid in dx11; e.g. IID_ID3D11ShaderReflection
#pragma comment(lib, "dxguid.lib")
