#pragma once

#include "HImGuiRenderDevice.h"

// @todo - note that imgui module should be merged with RHI!

namespace lsgd
{
	class HImGuiRenderDeviceDirectX12 : public HImGuiRenderDevice
	{
	public:
		HImGuiRenderDeviceDirectX12() {}
		virtual ~HImGuiRenderDeviceDirectX12() {}

		virtual bool Initialize() override;
		virtual void Tick() override;
		virtual void Render() override;

		bool CreateDeviceObjects();
		void DestroyDeviceObjects();
		void CreateFontsTexture();

		void RenderDrawData(uint32 InFrameIndex, ImDrawData* InDrawData, ID3D12GraphicsCommandList* InCommandList);
	};
}
