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

		void UpdateMousePos();
	};
}