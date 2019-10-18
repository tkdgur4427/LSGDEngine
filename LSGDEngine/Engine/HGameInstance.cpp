#include "HEnginePCH.h"
#include "HGameInstance.h"

#include "HHttpClient.h"

using namespace lsgd;

IMPLEMENT_CLASS_TYPE1(HGameInstance, HObject)

void HGameInstance::Reflect()
{
	
}

// HGameInstance

void HGameInstance::Initialize()
{

}

void HGameInstance::Destroy()
{

}

void HGameInstance::Tick(float DeltaTime)
{
	if (bExecuteHttpClientTest)
	{
		networking::HHttpClient::Test();
	}
}