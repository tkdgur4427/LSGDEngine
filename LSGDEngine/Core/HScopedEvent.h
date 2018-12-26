#pragma once

namespace lsgd { namespace thread {

	class HScopedEvent
	{
	public:
		HScopedEvent();
		~HScopedEvent();

		void Trigger();

		shared_ptr<HEvent> Get() { return Event; }

		shared_ptr<HEvent> Event;
	};

} }