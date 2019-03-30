#pragma once

namespace lsgd
{
	class HWindowFrame
	{
	public:
		HWindowFrame() {}
		virtual ~HWindowFrame() {}

		virtual void CreateWindowFrame() {}
		virtual void DestroyWindowFrame() {}
		virtual void ShowWindowFrame() {}

		virtual void* GetHandle() const { return nullptr; }
	};
}