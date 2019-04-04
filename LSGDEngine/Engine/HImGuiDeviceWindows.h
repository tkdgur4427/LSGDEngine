#pragma once

#include "HImGuiDevice.h"

namespace lsgd
{
	class HImGuiDeviceWindows : public HImGuiDevice
	{
	public:
		HImGuiDeviceWindows();
		virtual ~HImGuiDeviceWindows();

		virtual bool Initialize(void* InHandle) override;
		virtual void Tick() override;

		int64 UpdateMessage(void* InHandle, uint32 InMSG, uint32 InWParam, uint32 InLParam);
		bool UpdateMouseCursor();
		void UpdateMousePos();
	};
}

//@todo temporary...
extern lsgd::HImGuiDeviceWindows* GImGuiDeviceWindows;