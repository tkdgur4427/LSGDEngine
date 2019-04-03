#include "HCorePCH.h"
#include "HTaskGraph.h"

#include "HTaskThreadBase.h"

using namespace lsgd;
using namespace lsgd::async;

HGraphEvent::HGraphEvent()
{
	SubsequentList.SetClosed(false);
	check(SubsequentList.Num() == 0);
}

HGraphEvent::~HGraphEvent()
{
	check(SubsequentList.Num() == 0);
	SubsequentList.SetClosed(true);	
}

shared_ptr<HGraphEvent> HGraphEvent::CreateGraphEvent()
{
	return make_shared<HGraphEvent>();
}

bool HGraphEvent::AddSubsequent(shared_ptr<HBaseGraphTask> InSubsequent)
{
	return SubsequentList.Push(InSubsequent);
}

void HGraphEvent::DispatchSubsequent()
{
	// pop all the subsequent list
	HArray<shared_ptr<HBaseGraphTask>> Subsequents;
	SubsequentList.PopAll(Subsequents);

	// reverse order dispatching (conditional queuing)
	for (int32 Index = Subsequents.size() - 1; Index >= 0; --Index)
	{
		auto& Subsequent = Subsequents[Index];
		check(Subsequent->NumberOfPrerequisitiesOutstanding.GetValue() >= 1);
		Subsequent->NumberOfPrerequisitiesOutstanding.Decrement();
		Subsequent->ConditionalQueueTask();
	}
}

void HBaseGraphTask::SetupCompletePrerequisities(bool bUnlock)
{
	if (bUnlock)
	{
		ConditionalQueueTask();
	}
}

void HBaseGraphTask::ConditionalQueueTask()
{
	if (NumberOfPrerequisitiesOutstanding.GetValue() == 0)
	{
		if (bNamedThread)
		{
			QueueTaskToNamedThread();
		}
		else
		{
			QueueTask();
		}		
	}
}

void HBaseGraphTask::QueueTask()
{
	HTaskThreadBase::TaskThreadSharedContext.EnqueueGraphTask(shared_from_this());
}

void HBaseGraphTask::QueueTaskToNamedThread()
{
	check(bNamedThread);
	HTaskThreadBase::TaskThreadSharedContext.EnqueueGraphTaskToNamedThread(NamedThreadName, shared_from_this());
}