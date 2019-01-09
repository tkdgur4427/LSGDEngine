#pragma once

#include "HEBusContainer.h"
#include "HEHandlerContainer.h"
#include "HEBusCallQueue.h"

namespace lsgd
{
	template <class InterfaceType, class BusIdType, class EBusType>
	class HEBusContext
	{
	public:
		using BusType = HEHandlerContainer<InterfaceType, BusIdType>;
		using HandlerNodeType = typename BusType::EHandlerNode;
		using BusContainerType = HEBusContainer<BusType>;
		using BusCallQueueType = HEBusCallQueue<EBusType>;

		BusContainerType BusContainer;
		BusCallQueueType BusCallQueue;
	};

	template <class Interface, class Traits, class EBusType>
	class HEBContextPolicy
	{
	public:
		using InterfaceType = Interface;
		using BusIdType = typename Traits::BusIdType;
		using ContextType = HEBusContext<Interface, BusIdType, EBusType>;
		using HandlerNodeType = typename ContextType::HandlerNodeType;

		static ContextType& GetGlobalContext()
		{
			static ContextType GlobalContext;
			return GlobalContext;
		}
	};
}