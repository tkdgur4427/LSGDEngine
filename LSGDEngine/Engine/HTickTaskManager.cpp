#include "HEnginePCH.h"
#include "HTickTaskManager.h"

// world
#include "HWorld.h"
// tick task sequencer
#include "HTickTaskSequencer.h"

using namespace lsgd;

void HTickTaskManager::StartFrame(HWorld* InWorld, float InDeltaSeconds, const HArray<HLevel*>& LevelsToTick)
{
	// make sure that tick levels are empty
	check(LevelList.size() == 0);

	// update the state starting 'StartFrame'
	Context.World = InWorld;
	Context.ThreadName = "GameThread";
	Context.DeltaSeconds = InDeltaSeconds;

	HTickTaskSequencer::GetSingleton()->StartFrame();

	// fill the level list
	FillLevelList(LevelsToTick);

	// start frame and queue tick functions for all tick task levels
	for (auto& Level : LevelList)
	{
		Level->StartFrame(Context);
	}

	for (auto& Level : LevelList)
	{
		Level->QueueAllTicks();
	}
}

void HTickTaskManager::EndFrame()
{
	// trigger end frame for each tick task levels
	for (auto& Level : LevelList)
	{
		Level->EndFrame();
	}

	HTickTaskSequencer::GetSingleton()->EndFrame();

	// clear LevelList that are already ticked
	LevelList.clear();

	// reset the state
	Context.Reset();
}

void HTickTaskManager::FillLevelList(const HArray<class HLevel*>& Levels)
{
	// add level to ticks
	for (auto& Level : Levels)
	{
		LevelList.push_back(Level->TickTaskLevel);
	}
}

void HTickTaskManager::RunTickGroup()
{
	HTickTaskSequencer::GetSingleton()->ReleaseTickGroup();
}