#pragma once

#include "HTickTaskLevel.h"

namespace lsgd
{
	class HTickTaskManager
	{
	public:
		HTickTaskManager() {}
		~HTickTaskManager() {}

		void StartFrame(class HWorld* InWorld, float InDeltaSeconds, const HArray<class HLevel*>& LevelsToTick);
		void EndFrame();

		void RunTickGroup();

		void FillLevelList(const HArray<class HLevel*>& Levels);

		HArray<shared_ptr<HTickTaskLevel>> LevelList;
		HTickFunctionContext Context;

		// all tick function master container
		HArray<shared_ptr<HTickFunction>> AllTickFunctions;
	};
}