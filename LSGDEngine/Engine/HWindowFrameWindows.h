#pragma once

#include "HWindowFrame.h"

namespace lsgd
{
	class HWindowFrameWindows : public HWindowFrame
	{
	public:
		HWindowFrameWindows();
		virtual ~HWindowFrameWindows();

		virtual void CreateWindowFrame() override;
		virtual void DestroyWindowFrame() override;
		virtual void ShowWindowFrame() override;
		virtual void* GetHandle() const override
		{
			return Handle;
		}

		virtual bool UpdateMessage() override;

	protected:
		HWND Handle;
		WNDCLASSEX WindowClass;
	};
}