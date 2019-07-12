#include "HMMO_TcpFighterPCH.h"
#include "HGameInstanceOverride.h"

using namespace lsgd;

IMPLEMENT_CLASS_TYPE1(HGameInstanceOverride, HGameInstance)

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

	HGameInstance::Destroy();
}

void HGameInstanceOverride::Tick(float DeltaTime)
{
	HGameInstance::Tick(DeltaTime);

	IpDriver->Tick(DeltaTime);
}