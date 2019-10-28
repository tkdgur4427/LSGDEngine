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
	SGD_SCOPED_SIMPLE_PROFILER(HWorld_Tick);

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

HObjectHandleWeak<HLevel> HWorld::AddLevel()
{
	HObjectHandleUnique<HLevel> NewLevel(AllocateHObject(HLevel::GetClassName(), GPersistentPackage));

	// add new level to the current world
	Levels.push_back(HMove(NewLevel));

	HObjectHandleWeak<HLevel> Result(Levels.back()->GetObjectArrayData());
	return Result;
}

int32 HWorld::FindLevel(HObjectHandleWeak<HLevel> InLevel)
{
	int32 FoundIndex = -1;

	int32 CurrIndex = 0;
	for (auto& Level : Levels)
	{
		if (InLevel->Name.ToString() == Level->Name.ToString())
		{
			FoundIndex = CurrIndex;
			break;
		}

		CurrIndex++;
	}

	return FoundIndex;
}

void HWorld::RemoveLevel(HObjectHandleWeak<HLevel> InLevel)
{
	int32 FoundLevelIndex = FindLevel(InLevel);
	Levels.erase(Levels.begin() + FoundLevelIndex);
}
