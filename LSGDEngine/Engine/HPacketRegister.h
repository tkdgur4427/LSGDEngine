#pragma once

#include "HNetworkPacket.h"

namespace lsgd
{
	class HPacketGeneratorInterface
	{
	public:
		virtual ~HPacketGeneratorInterface() {}

		virtual HObjectHandleUnique<HNetworkPacket> CreatePacket() = 0;
	};

	template <class PacketType>
	class HPacketGenerator : public HPacketGeneratorInterface
	{
	public:
		virtual ~HPacketGenerator() {}

		virtual HObjectHandleUnique<HNetworkPacket> CreatePacket() override
		{
			HObjectHandleUnique<HNetworkPacket> NewPacket(AllocateHObject(PacketType::GetClassName(), GPersistentPackage));
			return HMove(NewPacket);
		}
	};

	class HPacketRegisterSystem
	{
	public:
		template <class PacketType>
		static void RegisterPacketGenerator()
		{
			if (GetPacketGeneratorContainer().find(PacketType::GetId()) != GetPacketGeneratorContainer().end())
			{
				check(false); // overlapped packet register happens
			}

			shared_ptr<HPacketGeneratorInterface> NewPacketGenerator = HMakeShared<HPacketGeneratorInterface, HPacketGenerator<PacketType>>();
			GetPacketGeneratorContainer().insert({PacketType::GetId(), NewPacketGenerator});
		}

		static HObjectHandleUnique<HNetworkPacket> CreatePacketWithId(uint16 Id)
		{
			auto PacketGenerator = GetPacketGeneratorContainer().find(Id);
			check(PacketGenerator != GetPacketGeneratorContainer().end());

			return HMove(PacketGenerator->second->CreatePacket());
		}

		typedef uint16 HPacketTypeId;
		typedef HHashMap<HPacketTypeId, shared_ptr<HPacketGeneratorInterface>> HPacketGeneratorContainer;

		static HPacketGeneratorContainer& GetPacketGeneratorContainer()
		{
			static HPacketGeneratorContainer Container;
			return Container;
		}
	};

	// packet registrator
	template <class PacketType>
	class HPacketRegistrator
	{
	public:
		HPacketRegistrator()
		{
			HPacketRegisterSystem::RegisterPacketGenerator<PacketType>();
		}
	};
}

#define REGISTER_PACKET_TYPE(Type) static lsgd::HPacketRegistrator<Type> PacketRegistrator_##Type;