#pragma once

namespace lsgd
{
	class HImGuiDevice
	{
	public:
		HImGuiDevice();
		virtual ~HImGuiDevice();

		virtual bool Initialize(void* InHandle) { return false; }
		virtual void Tick() {}

		void* Handle;
		int64 Time;
		int64 TicksPerSecond;
		ImGuiMouseCursor LastMouseCursor;
	};
}