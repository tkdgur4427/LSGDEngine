#include "HEnginePCH.h"
#include "HBaseEngine.h"

#include "HGameInstance.h"

using namespace lsgd;

void HBaseEngine::Initialize()
{
	auto Temp = reflect::HTypeDatabase::GetSingleton()->GetTypeName<HGameInstance>();
	//reflect::HTypeDescriptor ClassType = reflect::HTypeDatabaseUtils::GetTypeDescriptor<HGameInstance>();
	// create game instance
	//GameInstance = AllocateHObject<HGameInstance>();
	// add it to the root set
	//GameInstance.SetRoot();
}

void HBaseEngine::Destroy()
{
	// remove from the root set and ready to destroy
	GameInstance.UnsetRoot();
}

void HBaseEngine::Tick(float DeltaTime)
{

}