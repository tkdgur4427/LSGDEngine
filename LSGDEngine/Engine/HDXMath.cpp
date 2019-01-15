#include "HEnginePCH.h"
#include "HDXMath.h"

#include "DirectXSH.cpp"
#if defined(__d3d11_h__) || defined(__d3d11_x_h__)
#include "DirectXSHD3D11.cpp"
#endif
#if defined(__d3d12_h__) || defined(__d3d12_x_h__)
#include "DirectXSHD3D12.cpp"
#endif