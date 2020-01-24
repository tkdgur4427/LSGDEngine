#include "HCorePCH.h"
#include "HTaskGraphInterface.h"

// task thread runnable
#include "HTaskThreadBase.h"

#include "HProfiler.h"

using namespace lsgd;
using namespace lsgd::async;

HTaskGraphInterface* GTaskGraphInterface = nullptr;

void HTaskGraph1::Initialize()
{
	// generate task graph threads
	uint32 NumProcssors = HGenericPlatformMisc::GetHardwareThreadNum();
	
	// generate task threads 1x than number of processors
	uint32 TaskThreadNum = NumProcssors; 
	for (uint32 TaskThreadIndex = 0; TaskThreadIndex < TaskThreadNum; ++TaskThreadIndex)
	{
		HTaskThreadBase::TaskThreadSharedContext.CreateTaskThread();
	}

	// set the global task graph interface
	GTaskGraphInterface = this;
}

void HTaskGraph1::Destroy()
{

}

class FTriggerEventGraphTasks
{
public:	
	FTriggerEventGraphTasks(shared_ptr<HEvent> InEvent)
		: Event(InEvent)
	{}

	void Execute()
	{
		SGD_SCOPED_SIMPLE_PROFILER(FTriggerEventGraphTasks_Execute);
		Event->Trigger();
	}

	shared_ptr<HEvent> Event;
};

void HTaskGraph1::DispatchAndWaitUntilTaskComplete(shared_ptr<HBaseGraphTask>& Task, bool InbNamedThread, const HString& NamedThreadName)
{
	HScopedEvent Event;
	HArray<shared_ptr<HGraphEvent>> Tasks;
	Tasks.push_back(Task->GetCompletionEvent());
	
	HGraphTask<FTriggerEventGraphTasks>::CreateTask(Tasks, InbNamedThread, NamedThreadName).ConstructAndHold(Event.Get());
	Task->Unlock();
}
