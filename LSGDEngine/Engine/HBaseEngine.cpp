#include "HEnginePCH.h"
#include "HBaseEngine.h"

#include "HGameInstance.h"
#include "HDynamicRHI.h"

using namespace lsgd;

void HBaseEngine::Initialize()
{
	// create game instance
	GameInstance = HObjectHandleUnique<HGameInstance>(AllocateHObject(HGameInstance::GetClassName()));
	// add it to the root set
	GameInstance.SetRoot();
	// call initialize
	GameInstance->Initialize();

	// create window frame
	WindowFrame = HPlatformDeviceMisc::CreateWindowFrame();
	WindowFrame->CreateWindowFrame();
	WindowFrame->ShowWindowFrame();
	bWindowFrameMessageUpdated = false;

	// create RenderDevice
	GDynamicRHI = make_unique<lsgd::HDynamicRHI>();
	GDynamicRHI->CreateDevice(*WindowFrame);
}

void HBaseEngine::Destroy()
{
	// remove from the root set and ready to destroy
	GameInstance.UnsetRoot();
}

void HBaseEngine::Tick(float DeltaTime)
{
	// ticking game instance
	GameInstance->Tick(DeltaTime);

	// update message
	bWindowFrameMessageUpdated = WindowFrame->UpdateMessage();
}