#pragma once

// event bus
#include "HEBus.h"
// GCObject
#include "..\Core\HGCObject.h"

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

	template <typename PacketType>
	class HNetworkEventHandler : public HNetworkEventsBus::SingleEventHandlerType, public HGCObject
	{
	protected:
		HNetworkEventHandler(HObjectHandleUnique<PacketType> InPacket)
			// default constructor trigger
			: HGCObject()
			, HNetworkEventsBus::SingleEventHandlerType()
		{
			// compile-time checker
			static_assert(reflect::ExistClass<PacketType>() == true);
			Packet = HMove(InPacket);
		}

		virtual void AddReferencedObjects(gc::HReferenceCollector& Collector)
		{
			Collector << Packet;
		}

		HObjectHandleUnique<PacketType> Packet;
	};
}