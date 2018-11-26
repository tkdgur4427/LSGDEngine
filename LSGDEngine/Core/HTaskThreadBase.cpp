#include "HCorePCH.h"
#include "HTaskThreadBase.h"

using namespace lsgd::async;

// static declarations
HTaskThreadSharedContext HTaskThreadBase::TaskThreadSharedContext;

uint32 HTaskThreadSharedContext::GenerateTaskThreadId()
{
	return TaskThreadIdGenerator.Increment();
}
