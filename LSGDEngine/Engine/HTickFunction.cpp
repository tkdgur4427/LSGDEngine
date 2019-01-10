#include "HEnginePCH.h"
#include "HTickFunction.h"

// task graph
#include "..\Core\HTaskGraph.h"

// tick task sequencer
#include "HTickTaskSequencer.h"

using namespace lsgd;
using namespace lsgd::async;

HTickFunction::HTickFunction()
	: TaskPointer(nullptr)
	, TickState(ETickState::Disabled)
	, TickTaskLevel(nullptr)
	, TickInterval(0.f)
	, bRunOnTaskThreads(false)
{

}

HTickFunction::~HTickFunction()
{

}

shared_ptr<HGraphEvent> HTickFunction::GetCompletionHandle()
{
	check(TaskPointer);
	HGraphTask<HTickFunctionTask>* Task = (HGraphTask<HTickFunctionTask>*)(TaskPointer.get());
	return Task->GetCompletionEvent();
}

void HTickFunction::QueueTickFunction(HTickTaskSequencer& TTS, const HTickFunctionContext& InContext)
{
	check(InContext.ThreadName == "GameThread");

	if (TickState == HTickFunction::ETickState::Enabled)
	{
		// prepare the prerequisites (HGraphEvent)
		HArray<shared_ptr<HGraphEvent>> TaskPrerequisites;
		for (auto& Prerequisite : Prerequisites)
		{
			TaskPrerequisites.push_back(Prerequisite.PrerequisiteTickFunction->GetCompletionHandle());
		}

		// queue tick task
		shared_ptr<HTickFunction> SharedPtr(this); // holding shared_ptr ; note that enable_shared_from_this
		TTS.QueueTickTask(TaskPrerequisites, SharedPtr, InContext);
	}
}