#include "HEnginePCH.h"
#include "HTickTaskLevel.h"

#include "HTickTaskSequencer.h"

using namespace lsgd;

void HTickTaskLevel::StartFrame(HTickFunctionContext& InContext)
{
	// set the context
	TickContext.World = InContext.World;
	TickContext.ThreadName = InContext.ThreadName;
	TickContext.DeltaSeconds = InContext.DeltaSeconds;
}

void HTickTaskLevel::EndFrame()
{
	// reset the tick context for tick task level
	TickContext.Reset();
}

void HTickTaskLevel::QueueAllTicks()
{
	// queue all enabled tick functions
	for (auto& TickFunction : AllEnabledTickFunctions)
	{
		TickFunction->QueueTickFunction(*HTickTaskSequencer::GetSingleton(), TickContext);
	}
}

int32 HTickTaskLevel::FindTickFunction(shared_ptr<HTickFunction>& InTickFunction)
{
	int32 FoundIndex = 0;
	for (auto& TickFunction : AllEnabledTickFunctions)
	{
		if (TickFunction == InTickFunction)
		{
			return FoundIndex;
		}

		FoundIndex++;
	}

	return -1;
}

void HTickTaskLevel::RemoveTickFunction(shared_ptr<HTickFunction>& InTickFunction)
{
	int32 FindIndex = FindTickFunction(InTickFunction);
	check(FindIndex != -1);

	AllEnabledTickFunctions.erase(AllEnabledTickFunctions.begin() + FindIndex);
}