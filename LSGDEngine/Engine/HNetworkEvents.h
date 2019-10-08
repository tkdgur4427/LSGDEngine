#pragma once

// event bus
#include "HEBus.h"

namespace lsgd
{
	class HNetworkEvents : public HEBusTraits
	{
	public:
		HNetworkEvents() {}
		virtual ~HNetworkEvents() {}

		virtual void HandleEvent() = 0;

		using HandlerNodeType = shared_ptr<HNetworkEvents>;

		static void Execute(shared_ptr<HNetworkEvents> InNetworkEvents)
		{
			InNetworkEvents->HandleEvent();
		}
	};

	// the EBus for network events
	typedef HEBus<HNetworkEvents> HNetworkEventsBus;

	struct HNetworkEventHandler : public HNetworkEventsBus::SingleEventHandlerType
	{
		virtual void HandleEvent() override
		{
			// do something
		}
	};
}