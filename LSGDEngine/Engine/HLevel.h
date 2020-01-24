#pragma once

#include "HActor.h"

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HLevel, HObject)
	class HLevel : public HObject
	{
	public:
		GENERATE_CLASS_BODY(HLevel);

		HLevel();
		virtual ~HLevel() {}

		// add actor to the level
		HObjectHandleWeak<HActor> AddActor(HObjectHandleUnique<HActor>& InActor);
		void RemoveActor(HObjectHandleWeak<HActor>& InActor);
		int32 FindActor(HObjectHandleWeak<HActor>& InActor);

		HArray<HObjectHandleUnique<HActor>> Actors;

		// transient members
		shared_ptr<class HTickTaskLevel> TickTaskLevel;
	};
}