#include "HBattleServerPCH.h"
#include "HGameInstanceOverride.h"

// HLevel & HWorld
#include "..\Engine\HWorld.h"

#include "..\Engine\HTimerManager.h"

using namespace lsgd;

IMPLEMENT_CLASS_TYPE1(HGameInstanceOverride, HGameInstance)
IMPLEMENT_CLASS_TYPE1(HPACKET_CS_GAME_REQ_LOGIN, HNetworkPacket)
IMPLEMENT_CLASS_TYPE1(HPACKET_CS_GAME_RES_LOGIN, HNetworkPacket)
IMPLEMENT_CLASS_TYPE1(HPACKET_CS_GAME_REQ_ENTER_ROOM, HNetworkPacket)
IMPLEMENT_CLASS_TYPE1(HPACKET_CS_GAME_RES_ENTER_ROOM, HNetworkPacket)
IMPLEMENT_CLASS_TYPE1(HPACKET_CS_GAME_RES_PLAY_READY, HNetworkPacket)
IMPLEMENT_CLASS_TYPE1(HPACKET_CS_GAME_RES_PLAY_START, HNetworkPacket)
IMPLEMENT_CLASS_TYPE1(HPACKET_CS_GAME_RES_ADD_USER, HNetworkPacket)
IMPLEMENT_CLASS_TYPE1(HPACKET_CS_GAME_RES_REMOVE_USER, HNetworkPacket)
IMPLEMENT_CLASS_TYPE1(HPACKET_CS_GAME_RES_CREATE_MY_CHARACTER, HNetworkPacket)
IMPLEMENT_CLASS_TYPE1(HPACKET_CS_GAME_REQ_HEARTBEAT, HNetworkPacket)


// packet registration
REGISTER_PACKET_TYPE(HPACKET_CS_GAME_REQ_LOGIN)
REGISTER_PACKET_TYPE(HPACKET_CS_GAME_RES_LOGIN)
REGISTER_PACKET_TYPE(HPACKET_CS_GAME_REQ_ENTER_ROOM)
REGISTER_PACKET_TYPE(HPACKET_CS_GAME_RES_ENTER_ROOM)
REGISTER_PACKET_TYPE(HPACKET_CS_GAME_RES_PLAY_READY)
REGISTER_PACKET_TYPE(HPACKET_CS_GAME_RES_PLAY_START)
REGISTER_PACKET_TYPE(HPACKET_CS_GAME_RES_ADD_USER)
REGISTER_PACKET_TYPE(HPACKET_CS_GAME_RES_REMOVE_USER)
REGISTER_PACKET_TYPE(HPACKET_CS_GAME_RES_CREATE_MY_CHARACTER)
REGISTER_PACKET_TYPE(HPACKET_CS_GAME_REQ_HEARTBEAT)

HObjectHandleWeak<HIpDriver> HGameInstanceOverride::CachedIpDriver;
shared_ptr<HGameInstanceOverride> HBattleServerUtil::CachedGameInstanceOverride = nullptr;

void HGameInstanceOverride::Reflect()
{

}

void HPACKET_CS_GAME_REQ_LOGIN::Reflect()
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Type", &HPACKET_CS_GAME_REQ_LOGIN::Type);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("AccountNo", &HPACKET_CS_GAME_REQ_LOGIN::AccountNo);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("SessionKey", &HPACKET_CS_GAME_REQ_LOGIN::SessionKey);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("ConnectedToken", &HPACKET_CS_GAME_REQ_LOGIN::ConnectedToken);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Ver_Code", &HPACKET_CS_GAME_REQ_LOGIN::Ver_Code);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("ClientKey", &HPACKET_CS_GAME_REQ_LOGIN::ClientKey);
}

void HPACKET_CS_GAME_RES_LOGIN::Reflect()
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Type", &HPACKET_CS_GAME_RES_LOGIN::Type);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("AccountNo", &HPACKET_CS_GAME_RES_LOGIN::AccountNo);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Result", &HPACKET_CS_GAME_RES_LOGIN::Result);
}

void HPACKET_CS_GAME_REQ_ENTER_ROOM::Reflect()
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Type", &HPACKET_CS_GAME_REQ_ENTER_ROOM::Type);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("AccountNo", &HPACKET_CS_GAME_REQ_ENTER_ROOM::AccountNo);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("RoomNo", &HPACKET_CS_GAME_REQ_ENTER_ROOM::RoomNo);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("EnterToken", &HPACKET_CS_GAME_REQ_ENTER_ROOM::EnterToken);
}

void HPACKET_CS_GAME_RES_ENTER_ROOM::Reflect()
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Type", &HPACKET_CS_GAME_RES_ENTER_ROOM::Type);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("AccountNo", &HPACKET_CS_GAME_RES_ENTER_ROOM::AccountNo);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("RoomNo", &HPACKET_CS_GAME_RES_ENTER_ROOM::RoomNo);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("MaxUser", &HPACKET_CS_GAME_RES_ENTER_ROOM::MaxUser);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Result", &HPACKET_CS_GAME_RES_ENTER_ROOM::Result);
}

void HPACKET_CS_GAME_RES_PLAY_READY::Reflect()
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Type", &HPACKET_CS_GAME_RES_PLAY_READY::Type);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("RoomNo", &HPACKET_CS_GAME_RES_PLAY_READY::RoomNo);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("ReadySec", &HPACKET_CS_GAME_RES_PLAY_READY::ReadySec);
}

void HPACKET_CS_GAME_RES_PLAY_START::Reflect()
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Type", &HPACKET_CS_GAME_RES_PLAY_START::Type);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("RoomNo", &HPACKET_CS_GAME_RES_PLAY_START::RoomNo);
}

void HPACKET_CS_GAME_RES_ADD_USER::Reflect()
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Type", &HPACKET_CS_GAME_RES_ADD_USER::Type);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("RoomNo", &HPACKET_CS_GAME_RES_ADD_USER::RoomNo);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("AccountNo", &HPACKET_CS_GAME_RES_ADD_USER::AccountNo);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Nickname", &HPACKET_CS_GAME_RES_ADD_USER::Nickname);

	reflect::HTypeDatabase::GetSingleton()->AddClassField("Record_PlayCount", &HPACKET_CS_GAME_RES_ADD_USER::Record_PlayCount);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Record_PlayTime", &HPACKET_CS_GAME_RES_ADD_USER::Record_PlayTime);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Record_Kill", &HPACKET_CS_GAME_RES_ADD_USER::Record_Kill);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Record_Die", &HPACKET_CS_GAME_RES_ADD_USER::Record_Die);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Record_Win", &HPACKET_CS_GAME_RES_ADD_USER::Record_Win);
}

void HPACKET_CS_GAME_RES_REMOVE_USER::Reflect()
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Type", &HPACKET_CS_GAME_RES_REMOVE_USER::Type);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("RoomNo", &HPACKET_CS_GAME_RES_REMOVE_USER::RoomNo);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("AccountNo", &HPACKET_CS_GAME_RES_REMOVE_USER::AccountNo);
}

void HPACKET_CS_GAME_RES_CREATE_MY_CHARACTER::Reflect()
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Type", &HPACKET_CS_GAME_RES_CREATE_MY_CHARACTER::Type);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("PosX", &HPACKET_CS_GAME_RES_CREATE_MY_CHARACTER::PosX);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("PosY", &HPACKET_CS_GAME_RES_CREATE_MY_CHARACTER::PosY);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("HP", &HPACKET_CS_GAME_RES_CREATE_MY_CHARACTER::HP);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Bullet", &HPACKET_CS_GAME_RES_CREATE_MY_CHARACTER::Bullet);
}

void HPACKET_CS_GAME_REQ_HEARTBEAT::Reflect()
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Type", &HPACKET_CS_GAME_REQ_HEARTBEAT::Type);
}

void HGameInstanceOverride::Initialize()
{
	HGameInstance::Initialize();

	IpDriver = HObjectHandleUnique<HIpDriver>(AllocateHObject(HIpDriver::GetClassName()));
	IpDriver.SetRoot();

	// set port number as 6001
	IpDriver->Initialize(6001);

	// cache the IpDriver
	CachedIpDriver = HObjectHandleWeak<HIpDriver>(IpDriver->GetObjectArrayData());

	HBattleServerUtil::CachedGameInstanceOverride = shared_ptr<HGameInstanceOverride>(this);
}

void HGameInstanceOverride::Destroy()
{
	IpDriver->Destroy();

	HGameInstance::Destroy();
}

void HGameInstanceOverride::Tick(float DeltaTime)
{
	HGameInstance::Tick(DeltaTime);

	IpDriver->Tick(DeltaTime);
}

void HPACKET_CS_GAME_REQ_LOGIN::HandleEvent(class HNetConnection* InConnection)
{
	HObjectHandleUnique<HPACKET_CS_GAME_RES_LOGIN> PACKET_CS_MATCH_RES_LOGIN(AllocateHObject(HPACKET_CS_GAME_RES_LOGIN::GetClassName(), GPersistentPackage));

	// arbitrary send it as success
	PACKET_CS_MATCH_RES_LOGIN->AccountNo = AccountNo;
	PACKET_CS_MATCH_RES_LOGIN->Result = 1;

	// create memory archive
	HArray<uint8> SendData;
	HMemoryArchive Archive(SendData);
	Archive.bIsSaving = true;

	// serialize the packet into Archive
	PACKET_CS_MATCH_RES_LOGIN->Serialize(Archive);

	// send the packet data
	HGameInstanceOverride::CachedIpDriver->SendPacket(InConnection, SendData);
}

void HPACKET_CS_GAME_REQ_ENTER_ROOM::HandleEvent(class HNetConnection* InConnection)
{
	// @todo: currently, we get an available room not existing one, we need to fix now
	shared_ptr<HGameRoom> GameRoomToEnter = HBattleServerUtil::GetAvailableRoom();
	check(GameRoomToEnter->IsAvailable());

	// setting game room state
	GameRoomToEnter->GameRoomState = HGameRoom::Waiting;

	// update game room id
	RoomNo = GameRoomToEnter->Id;

	// add player to the game room
	HObjectHandleWeak<HNetConnection> NetConnection(InConnection->GetObjectArrayData());
	HBattleServerUtil::AddPlayerToGameRoom(GameRoomToEnter, NetConnection);

	HObjectHandleUnique<HPACKET_CS_GAME_RES_ENTER_ROOM> PACKET_CS_GAME_RES_ENTER_ROOM(AllocateHObject(HPACKET_CS_GAME_RES_ENTER_ROOM::GetClassName(), GPersistentPackage));	

	// arbitrary send it as success
	PACKET_CS_GAME_RES_ENTER_ROOM->AccountNo = AccountNo;
	PACKET_CS_GAME_RES_ENTER_ROOM->RoomNo = RoomNo;
	PACKET_CS_GAME_RES_ENTER_ROOM->MaxUser = 1;
	PACKET_CS_GAME_RES_ENTER_ROOM->Result = 1;

	// create memory archive
	HArray<uint8> SendData;
	HMemoryArchive Archive(SendData);
	Archive.bIsSaving = true;

	// serialize the packet into Archive
	PACKET_CS_GAME_RES_ENTER_ROOM->Serialize(Archive);
	
	// send the packet data
	HGameInstanceOverride::CachedIpDriver->SendPacket(InConnection, SendData);

	HObjectHandleUnique<HPACKET_CS_GAME_RES_ADD_USER> PACKET_CS_GAME_RES_ADD_USER(AllocateHObject(HPACKET_CS_GAME_RES_ADD_USER::GetClassName(), GPersistentPackage));

	// arbitrary send it as success
	PACKET_CS_GAME_RES_ADD_USER->AccountNo = AccountNo;
	PACKET_CS_GAME_RES_ADD_USER->RoomNo = RoomNo;
	//HGenericMemory::MemCopy(&PACKET_CS_GAME_RES_ADD_USER->Nickname[0], TEXT("Temp"), sizeof(TEXT("Temp")));

	PACKET_CS_GAME_RES_ADD_USER->Record_PlayCount = 1;
	PACKET_CS_GAME_RES_ADD_USER->Record_PlayTime = 1;
	PACKET_CS_GAME_RES_ADD_USER->Record_Kill = 1;
	PACKET_CS_GAME_RES_ADD_USER->Record_Die = 1;
	PACKET_CS_GAME_RES_ADD_USER->Record_Win = 1;

	// create memory archive
	HArray<uint8> SendData1;
	HMemoryArchive Archive1(SendData1);
	Archive1.bIsSaving = true;

	// serialize the packet into Archive
	PACKET_CS_GAME_RES_ADD_USER->Serialize(Archive1);

	// send the packet data
	HGameInstanceOverride::CachedIpDriver->SendPacket(InConnection, SendData1);

	HTimerUnifiedDelegate TimerDelegate;
	int32 RoomNumber = RoomNo;
	//TimerDelegate.FuncCallback = [RoomNumber, InConnection]() {
		HObjectHandleUnique<HPACKET_CS_GAME_RES_PLAY_READY> PACKET_CS_GAME_RES_PLAY_READY(AllocateHObject(HPACKET_CS_GAME_RES_PLAY_READY::GetClassName(), GPersistentPackage));

		// arbitrary send it as success
		PACKET_CS_GAME_RES_PLAY_READY->RoomNo = RoomNumber;
		PACKET_CS_GAME_RES_PLAY_READY->ReadySec = 0;

		// create memory archive
		HArray<uint8> SendData2;
		HMemoryArchive Archive2(SendData2);
		Archive2.bIsSaving = true;

		// serialize the packet into Archive
		PACKET_CS_GAME_RES_PLAY_READY->Serialize(Archive2);

		// send the packet data
		HGameInstanceOverride::CachedIpDriver->SendPacket(InConnection, SendData2);
		
		//// @todo - temporary start the game right away
		HObjectHandleUnique<HPACKET_CS_GAME_RES_PLAY_START> PACKET_CS_GAME_RES_PLAY_START(AllocateHObject(HPACKET_CS_GAME_RES_PLAY_START::GetClassName(), GPersistentPackage));

		// arbitrary send it as success
		PACKET_CS_GAME_RES_PLAY_START->RoomNo = RoomNumber;

		// create memory archive
		HArray<uint8> SendData3;
		HMemoryArchive Archive3(SendData3);
		Archive3.bIsSaving = true;

		// serialize the packet into Archive
		PACKET_CS_GAME_RES_PLAY_START->Serialize(Archive3);

		// send the packet data
		HGameInstanceOverride::CachedIpDriver->SendPacket(InConnection, SendData3);
	//};

	//HTimerHandle NewHandle;
	//GWorld->TimerManager->SetTimer(NewHandle, TimerDelegate, 5.0f, false, 0.0f);
	

	

	// @todo - temporary start the game right away	

	// get potential spawn locations
	HArray<HVector2> SpawnLocations = HBattleServerUtil::GetSpawnLocations();

	//@todo - add more functionality to the game room
	HObjectHandleUnique<HPACKET_CS_GAME_RES_CREATE_MY_CHARACTER> PACKET_CS_GAME_RES_CREATE_MY_CHARACTER(AllocateHObject(HPACKET_CS_GAME_RES_CREATE_MY_CHARACTER::GetClassName(), GPersistentPackage));

	PACKET_CS_GAME_RES_CREATE_MY_CHARACTER->PosX = SpawnLocations[0].x;
	PACKET_CS_GAME_RES_CREATE_MY_CHARACTER->PosY = SpawnLocations[0].y;

	PACKET_CS_GAME_RES_CREATE_MY_CHARACTER->HP = HBattleServerUtil::GetInitialHP();
	PACKET_CS_GAME_RES_CREATE_MY_CHARACTER->Bullet = HBattleServerUtil::GetInitialBulletCount();

	// create memory archive
	HArray<uint8> SendData4;
	HMemoryArchive Archive4(SendData4);
	Archive4.bIsSaving = true;

	// serialize the packet into Archive
	PACKET_CS_GAME_RES_CREATE_MY_CHARACTER->Serialize(Archive4);

	HGameInstanceOverride::CachedIpDriver->SendPacket(InConnection, SendData4);
}

int32 HBattleServerUtil::IssueGameRoomId()
{
	static int32 CurrGameRoomId = 1;
	return CurrGameRoomId++;
}

void HBattleServerUtil::CreateWorld()
{
	check(!GWorld.IsValid());

	// do not use GPersistPackage to prevent GC
	HObjectHandleUnique<HWorld> NewWorld(AllocateHObject(HWorld::GetClassName()));
	NewWorld->Initialize();

	GWorld = HMove(NewWorld);
}

HObjectHandleWeak<HLevel> HBattleServerUtil::CreateNewLevelForGameRoom()
{
	// @todo - um...
	if (!GWorld.IsValid())
	{
		CreateWorld();
	}

	HObjectHandleWeak<HLevel> Result = GWorld->AddLevel();
	return Result;
}

HArray<HVector2> HBattleServerUtil::GetSpawnLocations()
{
	HArray<HVector2> Result;
	for (int32 Count = 0; Count < 10; ++Count)
	{
		Result.push_back(HVector2(g_Data_Position[Count][0], g_Data_Position[Count][1]));
	}
	return Result;
}

int32 HBattleServerUtil::GetInitialBulletCount()
{
	return g_Data_Cartridge_Bullet;
}

int32 HBattleServerUtil::GetInitialHP()
{
	return g_Data_HP;
}

shared_ptr<HGameRoom> HBattleServerUtil::CreateGameRoom()
{
	shared_ptr<HGameRoom> NewGameRoom = HMakeShared<HGameRoom>();

	// issue game room
	NewGameRoom->Id = IssueGameRoomId();

	// create HLevel
	NewGameRoom->Level = CreateNewLevelForGameRoom();

	// add game room to cached game instance
	int32 NewIndex = CachedGameInstanceOverride->GameRooms.size();
	CachedGameInstanceOverride->GameRooms.push_back(NewGameRoom);

	return CachedGameInstanceOverride->GameRooms[NewIndex];
}

void HBattleServerUtil::AddPlayerToGameRoom(shared_ptr<HGameRoom> InGameRoom, HObjectHandleWeak<HNetConnection> InClient)
{
	// create an actor to live in the level
	HObjectHandleUnique<HActor> PlayerActor(AllocateHObject(HActor::GetClassName(), GPersistentPackage));

	// initialize the actor
	PlayerActor->Initialize();

	// add actor to the corresponding level
	HObjectHandleWeak<HActor> AddedActor = InGameRoom->Level->AddActor(HMove(PlayerActor));
	
	// create new pair and add new pair
	HGameRoom::HNetConnectionAndActorPair NewPair;
	NewPair.Actor = AddedActor;
	NewPair.NetConnection = InClient;
	InGameRoom->Players.push_back(NewPair);
}

shared_ptr<HGameRoom> HBattleServerUtil::FindGameRoom(HObjectHandleWeak<HNetConnection>& InConnection)
{
	check(CachedGameInstanceOverride != nullptr);

	// @todo - simply lookup with looping, need to optimize further later
	for (auto& GameRoom : CachedGameInstanceOverride->GameRooms)
	{
		for (auto& Player : GameRoom->Players)
		{
			if (Player.NetConnection->GetObjectArrayData() == InConnection->GetObjectArrayData())
			{
				return GameRoom;
			}
		}
	}

	return nullptr;
}

shared_ptr<HGameRoom> HBattleServerUtil::FindAvailableRoom()
{
	check(CachedGameInstanceOverride != nullptr);

	// @todo - simply lookup with looping, need to optimize further later
	for (auto& GameRoom : CachedGameInstanceOverride->GameRooms)
	{
		if (GameRoom->IsAvailable())
		{
			return GameRoom;
		}
	}

	return nullptr;
}

shared_ptr<HGameRoom> HBattleServerUtil::GetAvailableRoom()
{
	check(CachedGameInstanceOverride != nullptr);

	shared_ptr<HGameRoom> AvailableGameRoom = FindAvailableRoom();
	if (AvailableGameRoom == nullptr)
	{
		AvailableGameRoom = CreateGameRoom();
	}

	check(AvailableGameRoom != nullptr);
	return AvailableGameRoom;
}
