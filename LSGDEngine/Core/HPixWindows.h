#pragma once

#include "pix3.h"

namespace lsgd
{
	class HScopedPix
	{
	public:
		HScopedPix()
		{
			PIXBeginEvent(PIX_COLOR(255, 0, 0), "Unnamed");
		}

		HScopedPix(const char* InName)
		{
			PIXBeginEvent(PIX_COLOR(255, 0, 0), InName);
		}

		// varadic template arguments constructor for ScopedPix
		template <class... ArgsTypes>
		HScopedPix(const char* InFormatName, ArgsTypes... Args)
		{
			PIXBeginEvent(PIX_COLOR(255, 0, 0), InFormatName, Args...);
		}

		~HScopedPix()
		{
			PIXEndEvent();
		}
	};
}