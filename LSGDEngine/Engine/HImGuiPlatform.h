#pragma once

#include "HImGuiDevice.h"
#include "HImGuiRenderDevice.h"

namespace lsgd
{
	class HImGuiPlatform
	{
	public:
		HImGuiPlatform();
		~HImGuiPlatform();

		bool Initialize(void* InHandle);
		void Tick();
		void Render();

		unique_ptr<HImGuiDevice> ImGuiDevice;
		unique_ptr<HImGuiRenderDevice> ImGuiRenderDevice;
	};
}