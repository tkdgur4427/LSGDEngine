#include "HCorePCH.h"
#include "HEvent.h"

using namespace lsgd;
using namespace lsgd::thread;

HEvent::HEvent()
{
	EventObject = HMove(HGenericPlatformMisc::CreateEventObject());
}

HEvent::~HEvent()
{

}

void HEvent::Trigger()
{
	EventObject->Trigger();

#if EVENT_DEBUG
	IsTriggered = true;
#endif
}

void HEvent::Reset()
{
	EventObject->Reset();
}

bool HEvent::Wait(uint32 WaitTime)
{
	return EventObject->Wait(WaitTime);
}