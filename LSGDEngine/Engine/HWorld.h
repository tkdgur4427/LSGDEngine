#pragma once

#include "HLevel.h"

namespace lsgd
{	
	DECLARE_CLASS_TYPE1(HWorld, HObject)
	class HWorld : public HObject
	{
	public:
		GENERATE_CLASS_BODY(HWorld);

		HWorld() {}
		virtual ~HWorld() {}

		void Initialize();
		void Destroy();

		void Tick(float DeltaTime);

		// level utils
		HObjectHandleWeak<HLevel> AddLevel();
		int32 FindLevel(HObjectHandleWeak<HLevel> InLevel);
		void RemoveLevel(HObjectHandleWeak<HLevel> InLevel);

		// levels
		HArray<HObjectHandleUnique<HLevel>> Levels;

		// tick task manager
		shared_ptr<class HTickTaskManager> TickTaskManager;

		// timer manager
		shared_ptr<class HTimerManager> TimerManager;
	};
}

extern lsgd::HObjectHandleUnique<HWorld> GWorld;