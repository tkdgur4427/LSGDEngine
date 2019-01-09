#pragma once

#include "HPrimitiveSceneInfo.h"

namespace lsgd {

	// render-world; aligned to HWorld
	class HScene
	{
	public:
		HScene() {}
		~HScene() {}

		HArray<shared_ptr<HPrimitiveSceneInfo>> PrimitiveSceneInfos;
	};

}