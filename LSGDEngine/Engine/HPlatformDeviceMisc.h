#pragma once

#include "HWindowFrame.h"

namespace lsgd
{
	class HPlatformDeviceMisc
	{
	public:
		static unique_ptr<HWindowFrame> CreateWindowFrame();
	};
}