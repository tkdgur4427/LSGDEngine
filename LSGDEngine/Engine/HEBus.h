#pragma once

#include "HEBContextPolicy.h"
#include "HEConnectionPolicy.h"

namespace lsgd
{
	struct HEBusTraits
	{
		using BusIdType = uint32;
	};

	template <class Interface, class BusTraits, class ContextPolicy>
	class HEBusImpl
		: public ContextPolicy
		, public HEConnectionPolicy<ContextPolicy>
	{};

	/*
		Event Bus
			- basic class for event handling bus
	*/
	template <class Interface, class BusTraits = Interface>
	class HEBus
		: public HEBusImpl<Interface, BusTraits, HEBContextPolicy<Interface, BusTraits>>
	{
	public:
	};
}
