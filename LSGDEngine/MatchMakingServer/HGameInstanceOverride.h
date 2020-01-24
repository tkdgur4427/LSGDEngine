#pragma once

#include "..\Engine\HGameInstance.h"

// tcp ip driver
#include "..\Engine\HIpDriver.h"

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HPACKET_CS_MATCH_REQ_LOGIN, HNetworkPacket)
	class HPACKET_CS_MATCH_REQ_LOGIN : public HNetworkPacket
	{
	public:
		GENERATE_CLASS_BODY(HPACKET_CS_MATCH_REQ_LOGIN)

		static uint16 GetId() { return en_PACKET_TYPE::en_PACKET_CS_MATCH_REQ_LOGIN; }

		HPACKET_CS_MATCH_REQ_LOGIN() : Type(GetId()) {}
		virtual ~HPACKET_CS_MATCH_REQ_LOGIN() {}

		virtual void HandleEvent(class HNetConnection* InConnection) override;

		// packet data
		uint16 Type;
		int64 AccountNo;
		char SessionKey[64];
		uint32 Ver_Code;
	};

	DECLARE_CLASS_TYPE1(HPACKET_CS_MATCH_RES_LOGIN, HNetworkPacket)
	class HPACKET_CS_MATCH_RES_LOGIN : public HNetworkPacket
	{
	public:
		GENERATE_CLASS_BODY(HPACKET_CS_MATCH_RES_LOGIN)

		static uint16 GetId() { return en_PACKET_TYPE::en_PACKET_CS_MATCH_RES_LOGIN; }

		HPACKET_CS_MATCH_RES_LOGIN()
			: Type(GetId())
		{}
		virtual ~HPACKET_CS_MATCH_RES_LOGIN() 
		{
		}

		virtual void HandleEvent(class HNetConnection* InConnection) override {}

		// packet data
		uint16 Type;
		uint8 Status;
	};

	DECLARE_CLASS_TYPE1(HPACKET_CS_MATCH_REQ_GAME_ROOM, HNetworkPacket)
	class HPACKET_CS_MATCH_REQ_GAME_ROOM : public HNetworkPacket
	{
	public:
		GENERATE_CLASS_BODY(HPACKET_CS_MATCH_REQ_GAME_ROOM)

		static uint16 GetId() { return en_PACKET_TYPE::en_PACKET_CS_MATCH_REQ_GAME_ROOM; }

		HPACKET_CS_MATCH_REQ_GAME_ROOM()
			: Type(GetId())
		{}
		virtual ~HPACKET_CS_MATCH_REQ_GAME_ROOM()
		{
		}

		virtual void HandleEvent(class HNetConnection* InConnection) override;

		// packet data
		uint16 Type;
	};

	DECLARE_CLASS_TYPE1(HPACKET_CS_MATCH_RES_GAME_ROOM, HNetworkPacket)
	class HPACKET_CS_MATCH_RES_GAME_ROOM : public HNetworkPacket
	{
	public:
		GENERATE_CLASS_BODY(HPACKET_CS_MATCH_RES_GAME_ROOM)

		static uint16 GetId() { return en_PACKET_TYPE::en_PACKET_CS_MATCH_RES_GAME_ROOM; }

		HPACKET_CS_MATCH_RES_GAME_ROOM()
			: Type(GetId())
		{}
		virtual ~HPACKET_CS_MATCH_RES_GAME_ROOM()
		{
		}

		virtual void HandleEvent(class HNetConnection* InConnection) override {}

		// packet data
		uint16 Type;
		uint8 Status;
		uint16 BattleServerNo;
		wchar IP[16];
		uint16 Port;

		int32 RoomNo;
		char ConnectToken[32];
		char EnterToken[32];

		wchar ChatServerIP[16];
		uint16 ChatServerPort;

		int64 ClientKey;
	};

	DECLARE_CLASS_TYPE1(HPACKET_CS_MATCH_REQ_GAME_ROOM_ENTER, HNetworkPacket)
	class HPACKET_CS_MATCH_REQ_GAME_ROOM_ENTER : public HNetworkPacket
	{
	public:
		GENERATE_CLASS_BODY(HPACKET_CS_MATCH_REQ_GAME_ROOM_ENTER)

		static uint16 GetId() { return en_PACKET_TYPE::en_PACKET_CS_MATCH_REQ_GAME_ROOM_ENTER; }

		HPACKET_CS_MATCH_REQ_GAME_ROOM_ENTER() : Type(GetId()) {}
		virtual ~HPACKET_CS_MATCH_REQ_GAME_ROOM_ENTER() {}

		virtual void HandleEvent(class HNetConnection* InConnection) override;

		// packet data
		uint16 Type;

		uint16 BattleServerNo;
		int32 RoomNo;
	};

	DECLARE_CLASS_TYPE1(HPACKET_CS_MATCH_RES_GAME_ROOM_ENTER, HNetworkPacket)
	class HPACKET_CS_MATCH_RES_GAME_ROOM_ENTER : public HNetworkPacket
	{
	public:
		GENERATE_CLASS_BODY(HPACKET_CS_MATCH_RES_GAME_ROOM_ENTER)

		static uint16 GetId() { return en_PACKET_TYPE::en_PACKET_CS_MATCH_RES_GAME_ROOM_ENTER; }

		HPACKET_CS_MATCH_RES_GAME_ROOM_ENTER() : Type(GetId()) {}
		virtual ~HPACKET_CS_MATCH_RES_GAME_ROOM_ENTER() {}

		virtual void HandleEvent(class HNetConnection* InConnection) override {}

		// packet data
		uint16 Type;
	};

	DECLARE_CLASS_TYPE1(HGameInstanceOverride, HGameInstance)
	class HGameInstanceOverride : public HGameInstance
	{
	public:
		GENERATE_CLASS_BODY(HGameInstanceOverride)

		HGameInstanceOverride() {}
		virtual ~HGameInstanceOverride() {}

		virtual void Initialize() override;
		virtual void Destroy() override;
		virtual void Tick(float DeltaTime) override;

		HObjectHandleUnique<HIpDriver> IpDriver;

		static HObjectHandleWeak<HIpDriver> CachedIpDriver;
	};
}

