#pragma once

namespace lsgd
{
	template <class BusType>
	class HEBusContainer
	{
		using EBusIdType = typename BusType::EBusIdType;
		using EBusType = BusType;
		using EBusNode = shared_ptr<EBusType>;
		
		void Register(const EBusIdType& InBusId)
		{
			check(Buses.find(InBusId) == Buses.end());

			EBusNode NewNode = make_shared<EBusType>(InBusId);
			Buses.insert(InBusId, NewNode);
		}

		void Unregister(const EBusIdType& InBusId)
		{
			check(Buses.find(InBusId) != Buses.end());

			Buses.erase(InBusId);
		}
		
		HHashMap<EBusIdType, EBusNode> Buses;
	};
}
