#pragma once

using namespace lsgd;

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
		bool Wait(uint32 WaitTime);

	protected:
		unique_ptr<HPlatformEvent> EventObject;
	};

} }