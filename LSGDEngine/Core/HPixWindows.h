#pragma once

#include "pix3.h"

namespace lsgd
{
	class HScopedPix
	{
	public:
		HScopedPix(const char* InName)
		{
			PIXBeginEvent(PIX_COLOR(255, 0, 0), InName);
		}

		~HScopedPix()
		{
			PIXEndEvent();
		}
	};
}