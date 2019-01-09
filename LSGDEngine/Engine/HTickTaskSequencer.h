#pragma once

#include "HTickFunction.h"

// task graph for TickTaskSequencer implementation
#include "..\Core\HTaskGraph.h"

using namespace lsgd::async;

namespace lsgd
{
	class HTickFunctionTask
	{
	public:
		HTickFunction* Target;
		HTickFunctionContext Context;
	};

	class HTickTaskSequencer : public HSingletonTemplate<HTickTaskSequencer>
	{
	public:
		// completion handles for each phase of ticks
		HArray<shared_ptr<HGraphEvent>> TickCompletionEvents;

		// holded tasks
		HArray<shared_ptr<HGraphTask<HTickFunctionTask>>> TickFunctionTasks;
	};
}