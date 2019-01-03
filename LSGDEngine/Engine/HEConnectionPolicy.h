#pragma once

namespace lsgd
{
	template <class BusType>
	struct HEConnectionPolicy
	{
		using ContextType = BusType::Context;
		using BusIdType = BusType::BusIdType;

		static void Bind(const ContextType& InContext, const BusIdType& InBusId)
		{
			
		}
	};
}