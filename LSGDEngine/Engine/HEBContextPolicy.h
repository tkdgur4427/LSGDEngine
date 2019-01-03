#pragma once

#include "HEBusContainer.h"
#include "HEHandlerContainer.h"

namespace lsgd
{
	template <class InterfaceType, class BusIdType>
	class HEBusContext
	{
	public:
		using BusType = HEHandlerContainer<InterfaceType, BusIdType>;
		using HandlerNodeType = typename BusType::EHandlerNode;
		using BusContainerType = HEBusContainer<BusType>;

		BusContainerType BusContainer;
	};

	template <class Interface, class Traits>
	class HEBContextPolicy
	{
	public:
		using BusIdType = typename Traits::BusIdType;
		using ContextType = HEBusContext<Interface, BusIdType>;
		using HandlerNodeType = typename ContextType::HandlerNodeType;

		static ContextType& GetGlobalContext()
		{
			static ContextType GlobalContext;
			return GlobalContext;
		}
	};
}