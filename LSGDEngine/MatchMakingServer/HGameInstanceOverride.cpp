#include "HMatchMakingServerPCH.h"
#include "HGameInstanceOverride.h"

using namespace lsgd;

// implement HObject
IMPLEMENT_CLASS_TYPE1(HGameInstanceOverride, HGameInstance)
IMPLEMENT_CLASS_TYPE1(HPACKET_CS_MATCH_REQ_LOGIN, HObject)

// packet registration
REGISTER_PACKET_TYPE(HPACKET_CS_MATCH_REQ_LOGIN)

void HPACKET_CS_MATCH_REQ_LOGIN::Reflect()
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Type", &HPACKET_CS_MATCH_REQ_LOGIN::Type);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("AccountNo", &HPACKET_CS_MATCH_REQ_LOGIN::AccountNo);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("SessionKey", &HPACKET_CS_MATCH_REQ_LOGIN::SessionKey);
	reflect::HTypeDatabase::GetSingleton()->AddClassField("Ver_Code", &HPACKET_CS_MATCH_REQ_LOGIN::Ver_Code);
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
