#include "HCorePCH.h"
#include "HScopedEvent.h"
#include "HEvent.h"

using namespace lsgd;
using namespace lsgd::thread;

HScopedEvent::HScopedEvent()
{
	Event = make_shared<HEvent>();
}

HScopedEvent::~HScopedEvent()
{
	Event->Wait(0);
}

void HScopedEvent::Trigger()
{
	Event->Trigger();
}
