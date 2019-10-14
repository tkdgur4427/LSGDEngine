#include "HMatchMakingServerPCH.h"
#include "HGameInstanceOverride.h"

using namespace lsgd;

// implement HObject
IMPLEMENT_CLASS_TYPE1(HGameInstanceOverride, HGameInstance)
IMPLEMENT_CLASS_TYPE1(HPACKET_CS_MATCH_REQ_LOGIN, HNetworkPacket)
IMPLEMENT_CLASS_TYPE1(HPACKET_CS_MATCH_RES_LOGIN, HNetworkPacket)
IMPLEMENT_CLASS_TYPE1(HPACKET_CS_MATCH_REQ_GAME_ROOM, HNetworkPacket)

// packet registration
REGISTER_PACKET_TYPE(HPACKET_CS_MATCH_REQ_LOGIN)
REGISTER_PACKET_TYPE(HPACKET_CS_MATCH_RES_LOGIN)
REGISTER_PACKET_TYPE(HPACKET_CS_MATCH_REQ_GAME_ROOM)

HObjectHandleWeak<HIpDriver> HGameInstanceOverride::CachedIpDriver;

void HPACKET_CS_MATCH_REQ_LOGIN::Reflect()
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Type", &HPACKET_CS_MATCH_REQ_LOGIN::Type);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("AccountNo", &HPACKET_CS_MATCH_REQ_LOGIN::AccountNo);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("SessionKey", &HPACKET_CS_MATCH_REQ_LOGIN::SessionKey);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Ver_Code", &HPACKET_CS_MATCH_REQ_LOGIN::Ver_Code);
}

void HPACKET_CS_MATCH_RES_LOGIN::Reflect()
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Type", &HPACKET_CS_MATCH_RES_LOGIN::Type);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Status", &HPACKET_CS_MATCH_RES_LOGIN::Status);
}

void HPACKET_CS_MATCH_REQ_GAME_ROOM::Reflect()
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Type", &HPACKET_CS_MATCH_REQ_GAME_ROOM::Type);
}

void HGameInstanceOverride::Reflect()
{

}

void HGameInstanceOverride::Initialize()
{
	HGameInstance::Initialize();

	IpDriver = HObjectHandleUnique<HIpDriver>(AllocateHObject(HIpDriver::GetClassName()));
	IpDriver.SetRoot();

	IpDriver->Initialize();

	// cache the IpDriver
	CachedIpDriver = HObjectHandleWeak<HIpDriver>(IpDriver->GetObjectArrayData());
}

void HGameInstanceOverride::Destroy()
{
	IpDriver->Destroy();
	IpDriver.UnsetRoot();

	HGameInstance::Destroy();
}

void HGameInstanceOverride::Tick(float DeltaTime)
{
	HGameInstance::Tick(DeltaTime);

	IpDriver->Tick(DeltaTime);
}

void HPACKET_CS_MATCH_REQ_LOGIN::HandleEvent(class HNetConnection* InConnection)
{
	HObjectHandleUnique<HPACKET_CS_MATCH_RES_LOGIN> PACKET_CS_MATCH_RES_LOGIN(AllocateHObject(HPACKET_CS_MATCH_RES_LOGIN::GetClassName(), GPersistentPackage));

	// arbitrary send it as success
	PACKET_CS_MATCH_RES_LOGIN->Status = 1;

	// create memory archive
	HArray<uint8> SendData;
	HMemoryArchive Archive(SendData);
	Archive.bIsSaving = true;

	// serialize the packet into Archive
	PACKET_CS_MATCH_RES_LOGIN->Serialize(Archive);

	// send the packet data
	HGameInstanceOverride::CachedIpDriver->SendPacket(InConnection, SendData);
}

void HPACKET_CS_MATCH_REQ_GAME_ROOM::HandleEvent(class HNetConnection* InConnection)
{

}
