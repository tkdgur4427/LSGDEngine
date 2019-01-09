#pragma once

#include "HTickFunction.h"

namespace lsgd
{
	class HTickTaskLevel
	{
	public:
		struct HTickScheduleDetails
		{
			shared_ptr<HTickFunction> TickFunction;
			float CoolDown;
		};

		HArray<shared_ptr<HTickFunction>> AllEnabledTickFunctions;
		HArray<shared_ptr<HTickFunction>> AllCoolingDownTickFunctions;
		HArray<shared_ptr<HTickFunction>> AllDisabledTickFunctions;
		HArray<shared_ptr<HTickFunction>> NewlySpawnedTickFunctions;
		HTickFunctionContext TickContext;
	};
}