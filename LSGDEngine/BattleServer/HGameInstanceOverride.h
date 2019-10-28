#pragma once

#include "..\Engine\HGameInstance.h"

// tcp ip driver
#include "..\Engine\HIpDriver.h"

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HPACKET_CS_GAME_REQ_LOGIN, HNetworkPacket)
	class HPACKET_CS_GAME_REQ_LOGIN : public HNetworkPacket
	{
	public:
		GENERATE_CLASS_BODY(HPACKET_CS_GAME_REQ_LOGIN)

		static uint16 GetId() { return en_PACKET_TYPE::en_PACKET_CS_GAME_REQ_LOGIN; }

		HPACKET_CS_GAME_REQ_LOGIN() : Type(GetId()) {}
		virtual ~HPACKET_CS_GAME_REQ_LOGIN() {}

		virtual void HandleEvent(class HNetConnection* InConnection) override;

		// packet data
		uint16 Type;
		int64 AccountNo;
		char SessionKey[64];
		char ConnectedToken[32];
		uint32 Ver_Code;
		int64 ClientKey;
	};

	DECLARE_CLASS_TYPE1(HPACKET_CS_GAME_RES_LOGIN, HNetworkPacket)
	class HPACKET_CS_GAME_RES_LOGIN : public HNetworkPacket
	{
	public:
		GENERATE_CLASS_BODY(HPACKET_CS_GAME_RES_LOGIN)

		static uint16 GetId() { return en_PACKET_TYPE::en_PACKET_CS_GAME_RES_LOGIN; }

		HPACKET_CS_GAME_RES_LOGIN() : Type(GetId()) {}
		virtual ~HPACKET_CS_GAME_RES_LOGIN() {}

		virtual void HandleEvent(class HNetConnection* InConnection) override {}

		// packet data
		uint16 Type;
		int64 AccountNo;
		uint8 Result;
	};

	DECLARE_CLASS_TYPE1(HPACKET_CS_GAME_REQ_ENTER_ROOM, HNetworkPacket)
	class HPACKET_CS_GAME_REQ_ENTER_ROOM : public HNetworkPacket
	{
	public:
		GENERATE_CLASS_BODY(HPACKET_CS_GAME_REQ_ENTER_ROOM)

		static uint16 GetId() { return en_PACKET_TYPE::en_PACKET_CS_GAME_REQ_ENTER_ROOM; }

		HPACKET_CS_GAME_REQ_ENTER_ROOM() : Type(GetId()) {}
		virtual ~HPACKET_CS_GAME_REQ_ENTER_ROOM() {}

		virtual void HandleEvent(class HNetConnection* InConnection) override;

		// packet data
		uint16 Type;
		int64 AccountNo;
		int32 RoomNo;
		char EnterToken[32];
	};

	DECLARE_CLASS_TYPE1(HPACKET_CS_GAME_RES_ENTER_ROOM, HNetworkPacket)
	class HPACKET_CS_GAME_RES_ENTER_ROOM : public HNetworkPacket
	{
	public:
		GENERATE_CLASS_BODY(HPACKET_CS_GAME_RES_ENTER_ROOM)

		static uint16 GetId() { return en_PACKET_TYPE::en_PACKET_CS_GAME_RES_ENTER_ROOM; }

		HPACKET_CS_GAME_RES_ENTER_ROOM() : Type(GetId()) {}
		virtual ~HPACKET_CS_GAME_RES_ENTER_ROOM() {}

		virtual void HandleEvent(class HNetConnection* InConnection) override {}

		// packet data
		uint16 Type;
		int64 AccountNo;
		int32 RoomNo;
		uint8 MaxUser;
		uint8 Result;
	};

	DECLARE_CLASS_TYPE1(HPACKET_CS_GAME_RES_PLAY_READY, HNetworkPacket)
	class HPACKET_CS_GAME_RES_PLAY_READY : public HNetworkPacket
	{
	public:
		GENERATE_CLASS_BODY(HPACKET_CS_GAME_RES_PLAY_READY)

		static uint16 GetId() { return en_PACKET_TYPE::en_PACKET_CS_GAME_RES_PLAY_READY; }

		HPACKET_CS_GAME_RES_PLAY_READY() : Type(GetId()) {}
		virtual ~HPACKET_CS_GAME_RES_PLAY_READY() {}

		virtual void HandleEvent(class HNetConnection* InConnection) override {}

		// packet data
		uint16 Type;
		int32 RoomNo;
		uint8 ReadySec;
	};

	DECLARE_CLASS_TYPE1(HPACKET_CS_GAME_RES_PLAY_START, HNetworkPacket)
	class HPACKET_CS_GAME_RES_PLAY_START : public HNetworkPacket
	{
	public:
		GENERATE_CLASS_BODY(HPACKET_CS_GAME_RES_PLAY_START)

		static uint16 GetId() { return en_PACKET_TYPE::en_PACKET_CS_GAME_RES_PLAY_START; }

		HPACKET_CS_GAME_RES_PLAY_START() : Type(GetId()) {}
		virtual ~HPACKET_CS_GAME_RES_PLAY_START() {}

		virtual void HandleEvent(class HNetConnection* InConnection) override {}

		// packet data
		uint16 Type;
		int32 RoomNo;
	};

	DECLARE_CLASS_TYPE1(HPACKET_CS_GAME_RES_ADD_USER, HNetworkPacket)
	class HPACKET_CS_GAME_RES_ADD_USER : public HNetworkPacket
	{
	public:
		GENERATE_CLASS_BODY(HPACKET_CS_GAME_RES_PLAY_START)

		static uint16 GetId() { return en_PACKET_TYPE::en_PACKET_CS_GAME_RES_ADD_USER; }

		HPACKET_CS_GAME_RES_ADD_USER() : Type(GetId()) {}
		virtual ~HPACKET_CS_GAME_RES_ADD_USER() {}

		virtual void HandleEvent(class HNetConnection* InConnection) override {}

		// packet data
		uint16 Type;
		int32 RoomNo;
		int64 AccountNo;
		wchar Nickname[20];

		int32 Record_PlayCount;
		int32 Record_PlayTime;
		int32 Record_Kill;
		int32 Record_Die;
		int32 Record_Win;
	};

	DECLARE_CLASS_TYPE1(HPACKET_CS_GAME_RES_REMOVE_USER, HNetworkPacket)
	class HPACKET_CS_GAME_RES_REMOVE_USER : public HNetworkPacket
	{
	public:
		GENERATE_CLASS_BODY(HPACKET_CS_GAME_RES_REMOVE_USER)

		static uint16 GetId() { return en_PACKET_TYPE::en_PACKET_CS_GAME_RES_REMOVE_USER; }

		HPACKET_CS_GAME_RES_REMOVE_USER() : Type(GetId()) {}
		virtual ~HPACKET_CS_GAME_RES_REMOVE_USER() {}

		virtual void HandleEvent(class HNetConnection* InConnection) override {}

		// packet data
		uint16 Type;
		int32 RoomNo;
		int64 AccountNo;
	};

	// common...
	DECLARE_CLASS_TYPE1(HPACKET_CS_GAME_REQ_HEARTBEAT, HNetworkPacket)
	class HPACKET_CS_GAME_REQ_HEARTBEAT : public HNetworkPacket
	{
	public:
		GENERATE_CLASS_BODY(HPACKET_CS_GAME_RES_REMOVE_USER)

		static uint16 GetId() { return en_PACKET_TYPE::en_PACKET_CS_GAME_REQ_HEARTBEAT; }

		HPACKET_CS_GAME_REQ_HEARTBEAT() : Type(GetId()) {}
		virtual ~HPACKET_CS_GAME_REQ_HEARTBEAT() {}

		virtual void HandleEvent(class HNetConnection* InConnection) override {}

		// packet data
		uint16 Type;
	};

	/*
		I make the HGameRoom have HNetConnections
		for the game-play, I use HLevel
		make a relationship with HNetConnection with HActor for supporting gameplay
	*/

	// util to make connection between game-room
	class HBattleServerUtil
	{
	public:
		// create level and attach to the GWorld
		static HObjectHandleWeak<class HLevel> CreateNewLevelForGameRoom();
		// get the spawn locations (random position)
		HArray<HVector2> GetSpawnLocations() const;
	};

	// game room with level
	class HGameRoom
	{
	public:
		// make connection with netconnection and actor
		struct HNetConnectionAndActorPair
		{
			HObjectHandleWeak<class HNetConnection> NetConnection;
			HObjectHandleWeak<class HActor> Actor;
		};

		// note that we use netconnection as weak handle (unique handle is reside in HIpDriver)
		HArray<HNetConnectionAndActorPair> Players;

		// each game room have HLevel
		HObjectHandleWeak<class HLevel> Level;
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

