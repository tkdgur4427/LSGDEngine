#pragma once

namespace lsgd
{
	class HImGuiRenderDevice
	{
	public:
		HImGuiRenderDevice();
		virtual ~HImGuiRenderDevice();

		virtual bool Initialize() { return false; }
		virtual void Tick() {}
		virtual void Render() {}
	};
}