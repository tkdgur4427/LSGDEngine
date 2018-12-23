#include "HCorePCH.h"
#include "HTaskThreadBase.h"

// thread manager
#include "HThreadManager.h"

using namespace lsgd;
using namespace lsgd::async;

// static declarations
HTaskThreadSharedContext HTaskThreadBase::TaskThreadSharedContext;

uint32 HTaskThreadSharedContext::GenerateTaskThreadId()
{
	return TaskThreadIdGenerator.Increment();
}

uint32 HTaskThreadSharedContext::AllocTaskThreadLS()
{
	uint32 Result = TaskThreadLocalStorageIdGenerator.Increment();
	return Result;
}

shared_ptr<HThreadRunnable> HTaskThreadSharedContext::GetTaskThread()
{
	uint32 TaskThreadIndex = GetTaskThreadIndex();
	if (TaskThreadIndex == -1)
	{
		return nullptr;
	}

	return TaskThreads[GetTaskThreadIndex()];
}

shared_ptr<HBaseGraphTask> HTaskThreadSharedContext::GetNextTask()
{
	return DequeueGraphTask();
}

void HTaskThreadSharedContext::EnqueueGraphTask(shared_ptr<HBaseGraphTask> InTask)
{
	Tasks.Push(InTask);
}

shared_ptr<HBaseGraphTask> HTaskThreadSharedContext::DequeueGraphTask()
{
	return Tasks.Pop();
}

void HTaskThreadSharedContext::CreateTaskThread()
{
	// create task thread base
	shared_ptr<HThreadRunnable> TaskThreadRunnable = make_shared<HThreadRunnable, HTaskThreadBase>();
	TaskThreads.push_back(TaskThreadRunnable);

	// create hardware thread
	lsgd::thread::HThreadManager::GetSingleton()->CreateHardwareThread(TaskThreadRunnable);
}

void HTaskThreadSharedContext::CreateNamedThread(const HString& InName)
{
	// create task thread base
	shared_ptr<HThreadRunnable> NamedThreadRunnable = make_shared<HThreadRunnable, HNamedTaskThread>(InName);
	NamedThreads.push_back(NamedThreadRunnable);

	// create hardware thread
	lsgd::thread::HThreadManager::GetSingleton()->CreateHardwareThread(NamedThreadRunnable);
}

int32 HTaskThreadSharedContext::GetTaskThreadIndex()
{
	int32 Result = -1;

	int32 TaskThreadIndex = 0;
	for (auto&& TaskThread : TaskThreads)
	{
		if (TaskThread->IsCurrThread())
		{
			Result = TaskThreadIndex;
		}
		TaskThreadIndex++;
	}

	return Result;
}

HTaskThreadBase::HTaskThreadBase()
	: HThreadRunnable("TaskThread")
	, UniqueId(TaskThreadSharedContext.GenerateTaskThreadId())
{

}

HTaskThreadBase::~HTaskThreadBase()
{

}

void HTaskThreadBase::Run()
{
	while (!(bTerminate.GetValue()))
	{
		// get the graph task
		shared_ptr<HBaseGraphTask> NewTask = TaskThreadSharedContext.GetNextTask();

		if (NewTask == nullptr)
		{
			// if there is no available task, make it stalled (2ms)
			HGenericPlatformMisc::Sleep(0.002);
		}		
		else
		{
			// execute the task
			NewTask->Execute();

			// when it executes the new task, task should have refcount as 1
			//	- it means when it is out-of-scope, it must be destroyed!
			check(NewTask.use_count() == 1);
		}
	}
}

void HTaskThreadBase::Terminate()
{
	// terminate the task thread running
	bTerminate.Increment();
}

bool HTaskThreadBase::IsValidTTLSIndex(uint32 Index)
{
	bool bResult = true;
	if (TaskThreadLocalStorages.size() <= Index && Index < 0)
	{
		// if it is out of range
		bResult = false;
	}
	else
	{
		if (TaskThreadLocalStorages[Index] == nullptr)
		{
			// if the local storage is invalid
			bResult = false;
		}
	}
	return bResult;
}

HNamedTaskThread::HNamedTaskThread(const HString& InName)
	: HTaskThreadBase()
{
	// update the name
	Name = InName;
}

HNamedTaskThread::~HNamedTaskThread()
{

}

void HNamedTaskThread::Run()
{
	while (!(bTerminate.GetValue()))
	{
		// get the graph task (its own task from its own tasks)
		shared_ptr<HBaseGraphTask> NewTask = Tasks.Pop();

		// @todo ; if new task is nullptr, get job stealing from common tasks queue for task threads

		if (NewTask == nullptr)
		{
			// if there is no available task, make it stalled (2ms)
			HGenericPlatformMisc::Sleep(0.002);
		}
		else
		{
			// execute the task
			NewTask->Execute();

			// when it executes the new task, task should have refcount as 1
			//	- it means when it is out-of-scope, it must be destroyed!
			check(NewTask.use_count() == 1);
		}
	}
}
