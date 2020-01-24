#include "HEnginePCH.h"
#include "HTickTaskSequencer.h"

#include "../Core/HTaskGraphInterface.h"

using namespace lsgd;
using namespace lsgd::async; // study; note that using keyword only works in one file!

void HTickTaskSequencer::StartFrame()
{
	
}

void HTickTaskSequencer::EndFrame()
{
	// make sure that queued tick function tasks executed and empty
	check(TickFunctionTasks.size() == 0);
}

void HTickTaskSequencer::StartTickTask(HArray<shared_ptr<HGraphEvent>>& Prerequisites, shared_ptr<HTickFunction>& TickFunction, const HTickFunctionContext& InContext)
{
	HTickFunctionContext UseContext = InContext;

	if (TickFunction->bRunOnTaskThreads)
	{
		UseContext.ThreadName = "TaskThread";
	}
	else
	{
		check(UseContext.ThreadName == "GameThread");
	}

	// create task (note that it just hold the task)
	TickFunction->TaskPointer = HGraphTask<HTickFunctionTask>::CreateTask(Prerequisites).ConstructAndHold(TickFunction, InContext);
}

void HTickTaskSequencer::AddTickTaskCompletion(shared_ptr<HBaseGraphTask>& InTask)
{
	TickFunctionTasks.push_back(InTask);
	HGraphTask<HTickFunctionTask>* GraphTask = (HGraphTask<HTickFunctionTask>*)InTask.get();
	TickCompletionEvents.push_back(GraphTask->GetCompletionEvent());
}

void HTickTaskSequencer::QueueTickTask(HArray<shared_ptr<HGraphEvent>>& Prerequisites, shared_ptr<HTickFunction>& TickFunction, const HTickFunctionContext& InContext)
{
	StartTickTask(Prerequisites, TickFunction, InContext);
	AddTickTaskCompletion(TickFunction->TaskPointer);
}

void HTickTaskSequencer::DispatchTickGroup()
{
	// trigger the tick function tasks!
	for (auto& TickFunctionTask : TickFunctionTasks)
	{
		TickFunctionTask->Unlock();
	}
}

void HTickTaskSequencer::ResetTickGroup()
{
	// waiting until task is finished
	//GTaskGraphInterface->WaitUntilTasksComplete(TickCompletionEvents);

	// clearing completion events
	for (auto& CompletionEvent : TickCompletionEvents)
	{
		check(CompletionEvent->ReferenceCount.GetValue() == 0);
	}
	TickCompletionEvents.clear();
}

void HTickTaskSequencer::ReleaseTickGroup()
{
	HArray<shared_ptr<HGraphEvent>> Prerequisites;
	HArray<shared_ptr<HGraphEvent>> WaitEvents;
	
	// 1. create dispatch tick group
	shared_ptr<HBaseGraphTask> DispatchTickGroupTask = HGraphTask<HDispatchTickGroupTask>::CreateTask(Prerequisites).ConstructAndHold();
	
	// 2. create reset tick group 
	Prerequisites.push_back(DispatchTickGroupTask->GetCompletionEvent());
	shared_ptr<HGraphEvent> ResetTickGroupTaskEvent = HGraphTask<HResetTickGroupTask>::CreateTask(Prerequisites).ConstructAndDispatchWhenReady();
	// add reset tick group event as waiting tasks complete
	WaitEvents.push_back(ResetTickGroupTaskEvent);

	// waiting until it finishes HResetTickGroupTask
	GTaskGraphInterface->DispatchAndWaitUntilTaskComplete(DispatchTickGroupTask);
}