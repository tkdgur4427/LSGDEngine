#include "HEnginePCH.h"
#include "HBaseEngine.h"

#include "HGameInstance.h"

using namespace lsgd;

void HBaseEngine::Initialize()
{
	HString name = HGameInstance::GetClassName();

	// create game instance
	//GameInstance = AllocateHObject<HGameInstance>();
	GameInstance = HObjectHandleUnique<HGameInstance>(AllocateHObject("HGameInstance"));
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