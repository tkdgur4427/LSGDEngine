#include "HLobbyServerPCH.h"
#include "HGameInstanceOverride.h"

using namespace lsgd;

IMPLEMENT_CLASS_TYPE1(HGameInstanceOverride, HGameInstance)

void HGameInstanceOverride::Reflect()
{

}

void HGameInstanceOverride::Initialize()
{
	HGameInstance::Initialize();
}

void HGameInstanceOverride::Destroy()
{
	HGameInstance::Destroy();
}

void HGameInstanceOverride::Tick(float DeltaTime)
{
	bExecuteHttpClientTest = true;

	HGameInstance::Tick(DeltaTime);
}
