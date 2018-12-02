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
	SubsequentList.SetClosed(true);
	check(SubsequentList.Num() == 0);
}

shared_ptr<HGraphEvent> HGraphEvent::CreateGraphEvent()
{
	return make_shared<HGraphEvent>();
}

bool HGraphEvent::AddSubsequent(shared_ptr<HBaseGraphTask>& InSubsequent)
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
		QueueTask();
	}
}

void HBaseGraphTask::QueueTask()
{
	HTaskThreadBase::TaskThreadSharedContext.EnqueueGraphTask(shared_from_this());
}