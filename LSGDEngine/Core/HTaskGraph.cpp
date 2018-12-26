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

void HBaseGraphTask::SetupCompletePrerequisities()
{
	ConditionalQueueTask();
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