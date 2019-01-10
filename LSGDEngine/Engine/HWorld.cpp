#include "HEnginePCH.h"
#include "HWorld.h"

#include "HTickTaskManager.h"

using namespace lsgd;

HWorld* GWorld = nullptr;

IMPLEMENT_CLASS_TYPE1(HWorld, HObject)

void HWorld::Reflect(){}

void HWorld::Initialize()
{
	check(GWorld == nullptr);

	// create tick task manager
	TickTaskManager = make_shared<HTickTaskManager>();

	GWorld = this;
}

void HWorld::Destroy()
{
	GWorld = nullptr;
}

void HWorld::Tick(float DeltaTime)
{
	// get the levels
	HArray<HLevel*> LevelsToTick;
	for (auto& LevelHandle : Levels)
	{
		if (LevelHandle)
		{
			LevelsToTick.push_back(LevelHandle.Get());
		}
	}

	TickTaskManager->StartFrame(this, DeltaTime, LevelsToTick);

	TickTaskManager->RunTickGroup();

	TickTaskManager->EndFrame();
}
