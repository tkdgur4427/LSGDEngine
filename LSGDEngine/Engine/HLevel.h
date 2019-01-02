#pragma once

#include "HActor.h"

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HLevel, HObject)
	class HLevel : public HObject
	{
	public:
		GENERATE_CLASS_BODY(HLevel);

		HLevel() {}
		virtual ~HLevel() {}

		HArray<HObjectHandleUnique<HActor>> Actors;
	};
}