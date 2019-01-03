#pragma once

#include "HEBContextPolicy.h"

namespace lsgd
{
	struct HEBusTraits
	{
		
	};

	template <class Traits, class BusType>
	class HEBusImpl 
		: public Traits
		, public HEBContextPolicy<BusType>
	{
	public:
	};

	/*
		Event Bus
			- basic class for event handling bus
	*/
	template <class Traits>
	class HEBus : public HEBusImpl<Traits, HEBus<Traits>>
	{
	public:	
	};
}
