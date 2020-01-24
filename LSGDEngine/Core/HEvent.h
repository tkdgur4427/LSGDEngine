#pragma once

using namespace lsgd;

#define EVENT_DEBUG 1

namespace lsgd {

	class HPlatformEvent;

}

namespace lsgd { namespace thread {

	class HEvent
	{
	public:
		HEvent();
		~HEvent();

		void Trigger();
		void Reset();
		bool Wait(uint32 WaitTime = -1);

	protected:
		unique_ptr<HPlatformEvent> EventObject;
		
#if EVENT_DEBUG
		bool IsTriggered = false;
#endif
	};

} }