#include "HEnginePCH.h"
#include "HWorld.h"

#include "HTickTaskManager.h"
#include "HTimerManager.h"

using namespace lsgd;

lsgd::HObjectHandleUnique<HWorld> GWorld;

IMPLEMENT_CLASS_TYPE1(HWorld, HObject)

void HWorld::Reflect(){}

void HWorld::Initialize()
{
	// create tick task manager
	TickTaskManager = make_shared<HTickTaskManager>();

	TimerManager = HMakeShared<HTimerManager>();
}

void HWorld::Destroy()
{
	
}

void HWorld::Tick(float DeltaTime)
{
	SGD_SCOPED_SIMPLE_PROFILER(HWorld_Tick);

	// timer manager
	TimerManager->Tick(DeltaTime);

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
