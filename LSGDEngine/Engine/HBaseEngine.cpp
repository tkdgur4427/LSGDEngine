#include "HEnginePCH.h"
#include "HBaseEngine.h"

#include "HGameInstance.h"

using namespace lsgd;

void HBaseEngine::Initialize()
{
	// create game instance
	GameInstance = HObjectHandleUnique<HGameInstance>(AllocateHObject(HGameInstance::GetClassName()));
	// add it to the root set
	GameInstance.SetRoot();
}

void HBaseEngine::Destroy()
{
	// remove from the root set and ready to destroy
	GameInstance.UnsetRoot();
}

void HBaseEngine::Tick(float DeltaTime)
{

}