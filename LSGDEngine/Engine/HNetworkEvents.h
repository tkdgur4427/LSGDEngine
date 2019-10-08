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

		// handler node type with shared_ptr (managed by reference counting)
		using HandlerNodeType = shared_ptr<HNetworkEvents>;

		// holder to execute network events
		static void Execute(HObjectHandleWeak<class HNetConnection> NetConnection, shared_ptr<HNetworkEvents> InNetworkEvents);

	protected:
		virtual void HandleEvent(class HNetConnection*) = 0;
	};

	// the EBus for network events
	typedef HEBus<HNetworkEvents> HNetworkEventsBus;

	class HNetworkEventHandler : public HNetworkEventsBus::SingleEventHandlerType
	{
	protected:
		// note that still need to override HandleEvent()
	};
}