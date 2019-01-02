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

		HArray<HObjectHandleUnique<HLevel>> Levels;
	};
}