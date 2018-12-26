#pragma once

#include "HTaskThreadBase.h"

namespace lsgd {

	class HCoreTlsData
	{
	public:
		HObjectInitializer ObjectInitializer;
	};

	typedef async::HTaskThreadSingleton<HCoreTlsData> HCoreTls;
}
