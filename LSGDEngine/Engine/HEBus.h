#pragma once

#include "HEBContextPolicy.h"
#include "HEConnectionPolicy.h"
#include "HEBroadCasterPolicy.h"

namespace lsgd
{
	class HEBusTraits : public enable_shared_from_this<HEBusTraits>
	{
	public:
		using BusIdType = uint32;

		HEBusTraits() {}
		virtual ~HEBusTraits() {}
	};

	// inherited interface type (singe/multiple)
	template <class BusType>
	class HEBusEventHandlerBase : public BusType::InterfaceType
	{
	public:
		struct HBusConnector
		{
			typename BusType::BusIdType BusId;
			typename BusType::HandlerNodeType HandlerNode;
		};

		HEBusEventHandlerBase() {}
		virtual ~HEBusEventHandlerBase() {}
	};

	template <class BusType>
	class HEBusSingleEventHandler : public HEBusEventHandlerBase<BusType>
	{
	public:
		using BaseType = HEBusEventHandlerBase<BusType>;
		using BusConnectorType = typename BaseType::HBusConnector;
		using BusIdType = typename BusType::BusIdType;

		HEBusSingleEventHandler() {}
		virtual ~HEBusSingleEventHandler() 
		{
			if (IsConnected())
			{
				BusType::Disconnect(Connector.BusId, Connector.HandlerNode);
			}
		}

		bool IsConnected() const { return Connector.HandlerNode; }

		void Connect(BusIdType InBusId = 0)
		{
			check(!IsConnected()); // only allowed connect once!

			Connector.BusId = InBusId;
			Connector.HandlerNode = this; // note that interface-type should inherits from 'enable_shared_from_this'

			BusType::BindAndConnect(Connector.BusId, Connector.HandlerNode);
		}

		BusConnectorType Connector;
	};

	template <class BusType>
	class HEBusMultipleEventHandler : public HEBusEventHandlerBase<BusType>
	{
	public:
		using BaseType = HEBusEventHandlerBase<BusType>;
		using BusConnectorType = BaseType::HBusConnector;
		using BusIdType = typename BusType::BusIdType;

		HEBusMultipleEventHandler() {}
		virtual ~HEBusMultipleEventHandler() 
		{
			Reset();
		}

		void Reset()
		{
			for (auto& Connector : Connectors)
			{
				BusType::Disconnect(Connector.BusId, Connector.HandlerNode);
			}
			Connectors.clear();
		}

		void Connect(BusIdType InBusId = 0)
		{
			check(!IsConnectedBus(InBusId)); // only allowed different bus line

			BusConnectorType NewConnector;
			NewConnector.BusId = InBusId;
			NewConnector.HandlerNode = this; // note that interface-type should inherits from 'enable_shared_from_this'

			BusType::BindAndConnect(NewConnector.BusId, NewConnector.HandlerNode);

			// add connectors
			Connectors.push_back(NewConnector);
		}

		bool IsConnectedBus(BusIdType InBusId)
		{
			for (auto& Connector : Connectors)
			{
				if (Connector.BusId == InBusId)
				{
					return true;
				}
			}
			return false;
		}

		HArray<BusConnectorType> Connectors;
	};

	template <class Interface, class BusTraits, class ContextPolicy>
	class HEBusImpl
		: public ContextPolicy
		, public HEConnectionPolicy<ContextPolicy>
		, public HEBroadCasterPolicy<ContextPolicy>
		, public HEBusCallQueue<ContextPolicy>
	{};

	/*
		Event Bus
			- basic class for event handling bus
	*/
	template <class Interface, class BusTraits = Interface>
	class HEBus
		: public HEBusImpl<Interface, BusTraits, HEBContextPolicy<Interface, BusTraits, HEBus<Interface, BusTraits>>>
	{
	public:
		// this type
		using ThisType = HEBus<Interface, BusTraits>;

		// type definition for HEBus class accessor
		//	- e.g. HEBusEventHandlerBase
		using BusIdType = typename BusTraits::BusIdType;
		using InterfaceType = Interface;

		using ContextPolicy = HEBContextPolicy<Interface, BusTraits, ThisType>;
		using HandlerNodeType = typename ContextPolicy::HandlerNodeType;

		// handler type definitions
		using SingleEventHandlerType = HEBusSingleEventHandler<ThisType>;
		using MultipleEventHandlerType = HEBusMultipleEventHandler<ThisType>;
	};
}
