#pragma once

#include "HTickTaskLevel.h"

namespace lsgd
{
	class HTickTaskManager
	{
	public:
		HArray<shared_ptr<HTickTaskLevel>> LevelList;
		HTickFunctionContext Context;

		// all tick function master container
		HArray<shared_ptr<HTickFunction>> AllTickFunctions;
	};
}