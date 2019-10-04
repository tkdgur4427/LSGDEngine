#include "HMatchMakingServerPCH.h"
#include "HGameInstanceOverride.h"

using namespace lsgd;

IMPLEMENT_CLASS_TYPE1(HGameInstanceOverride, HGameInstance)

IMPLEMENT_CLASS_TYPE1(HIocpDummy, HObject)

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

void HIocpDummy::Reflect()
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField<HIocpDummy>("Data", &HIocpDummy::Data);
}