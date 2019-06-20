#include "HEnginePCH.h"
#include "HNetConnection.h"

using namespace lsgd;

IMPLEMENT_CLASS_TYPE1(HNetConnection, HPlayer)

void HNetConnection::Reflect()
{

}

// HNetConnection

void HNetConnection::Initialize()
{
	HPlayer::Initialize();
}

void HNetConnection::Destroy()
{
	HPlayer::Destroy();
}

void HNetConnection::Tick(float DeltaTime)
{
	HPlayer::Tick(DeltaTime);
}