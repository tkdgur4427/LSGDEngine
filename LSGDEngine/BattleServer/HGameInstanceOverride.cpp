#include "HBattleServerPCH.h"
#include "HGameInstanceOverride.h"

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
REGISTER_PACKET_TYPE(HPACKET_CS_GAME_REQ_HEARTBEAT)

HObjectHandleWeak<HIpDriver> HGameInstanceOverride::CachedIpDriver;

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
}