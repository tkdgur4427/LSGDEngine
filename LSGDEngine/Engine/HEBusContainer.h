#pragma once

namespace lsgd
{
	template <class BusType>
	class HEBusContainer
	{
	public:
		using EBusIdType = typename BusType::EBusIdType;
		using EBusType = BusType;
		using EBusNode = shared_ptr<EBusType>;
		using Iterator = typename HHashMap<EBusIdType, EBusNode>::iterator;

		void Register(const EBusIdType& InBusId)
		{
			check(!Exist(InBusId));
			EBusNode NewNode = make_shared<EBusType>(InBusId);
			Buses.insert({ InBusId, NewNode });
		}

		void Unregister(const EBusIdType& InBusId)
		{
			check(Exist(InBusId));
			Buses.erase(InBusId);
		}

		bool Exist(const EBusIdType& InBusId)
		{
			return Buses.find(InBusId) != Buses.end();
		}

		Iterator Begin() { return Buses.begin(); }
		Iterator End() { return Buses.end(); }
		uint32 Num() const { return Buses.size(); }
		
		HHashMap<EBusIdType, EBusNode> Buses;
	};
}
