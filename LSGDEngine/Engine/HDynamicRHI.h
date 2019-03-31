#pragma once

#include "HWindowFrame.h"

namespace lsgd
{
	class HDynamicRHI
	{
	public:
		HDynamicRHI() {}
		~HDynamicRHI() {}

		void RenderBegin();
		void RenderEnd();
		void* GetCommandList();
		uint32 GetFrameIndex() const;

		void* GetDevice();
		void* GetSrvDescriptionHeap();

		bool CreateDevice(const HWindowFrame& InWindowFrame);
		void CleanupDevice();

		void WaitForLastSubmittedFrame();
		void CleanupRenderTarget();

		// @todo - need to think about how to make it performent
		void CreateVertexShader(const HArray<uint8>& InCode);
		void CreatePixelShader(const HArray<uint8>& InCode);
	};
}

extern unique_ptr<lsgd::HDynamicRHI> GDynamicRHI;

#define SAFE_RELEASE(Resource) \
	if (Resource) { Resource->Release(); Resource = nullptr; }