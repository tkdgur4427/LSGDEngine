#pragma once

#include "..\Engine\HGameInstance.h"

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HGameInstanceOverride, HGameInstance)
	class HGameInstanceOverride : public HGameInstance
	{
	public:
		GENERATE_CLASS_BODY(HGameInstanceOverride)

		HGameInstanceOverride() {}
		virtual ~HGameInstanceOverride() {}
	};
}

