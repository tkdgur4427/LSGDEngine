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