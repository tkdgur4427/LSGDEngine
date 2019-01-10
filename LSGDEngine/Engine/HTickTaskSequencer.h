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
		HTickFunctionTask(shared_ptr<HTickFunction>& InTaskFunction, const HTickFunctionContext& InContext)
			: Target(InTaskFunction)
		{
			Context = InContext;
		}

		void Execute()
		{
			Target->Execute();
		}

		shared_ptr<HTickFunction> Target;
		HTickFunctionContext Context;
	};

	class HTickTaskSequencer : public HSingletonTemplate<HTickTaskSequencer>
	{
	public:
		HTickTaskSequencer() {}
		virtual ~HTickTaskSequencer() {}

		// task that handles dispatching a tick group
		class HDispatchTickGroupTask
		{
		public:
			void Execute()
			{
				HTickTaskSequencer::GetSingleton()->DispatchTickGroup();
			}
		};

		// task that handles reset tick group
		class HResetTickGroupTask
		{
		public:
			void Execute()
			{
				HTickTaskSequencer::GetSingleton()->ResetTickGroup();
			}
		};

		void StartFrame();
		void EndFrame();

		void ReleaseTickGroup();

		// inner method triggered by tasks
		void DispatchTickGroup();
		void ResetTickGroup();

		// start tick task
		void StartTickTask(HArray<shared_ptr<HGraphEvent>>& Prerequisites, shared_ptr<HTickFunction>& TickFunction, const HTickFunctionContext& InContext);

		// add a completion handle to a tick group
		void AddTickTaskCompletion(shared_ptr<HBaseGraphTask>& InTask);

		// queue tick task
		void QueueTickTask(HArray<shared_ptr<HGraphEvent>>& Prerequisites, shared_ptr<HTickFunction>& TickFunction, const HTickFunctionContext& InContext);

		// completion handles for each phase of ticks
		HArray<shared_ptr<HGraphEvent>> TickCompletionEvents;

		// hold tasks
		HArray<shared_ptr<HBaseGraphTask>> TickFunctionTasks;
	};
}