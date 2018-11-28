#include "HCorePCH.h"
#include "HTaskThreadBase.h"

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
	: UniqueId(TaskThreadSharedContext.GenerateTaskThreadId())
{

}

HTaskThreadBase::~HTaskThreadBase()
{

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
