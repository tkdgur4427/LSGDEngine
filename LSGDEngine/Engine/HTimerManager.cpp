#include "HEnginePCH.h"
#include "HTimerManager.h"

using namespace lsgd;

int64 HTimerManager::LastAssignedSerialNumber = 0;

bool HTimerHandlePredicate::operator()(HTimerHandle& TimerHandle0, HTimerHandle& TimerHandle1)
{
	HTimerData& TimerData0 = TimerManager->GetTimer(TimerHandle0);
	HTimerData& TimerData1 = TimerManager->GetTimer(TimerHandle1);

	return TimerData0.ExpireTime > TimerData1.ExpireTime;
}

void HTimerManager::Tick(float DeltaTime)
{
	InternalTime += DeltaTime;

	while (ActiveTimers.size())
	{
		HTimerHandle TopHandle = ActiveTimers.top();
		HTimerData& TopData = Timers[TopHandle.GetIndex()];

		// process pending removal first!
		if (TopData.Status == HTimerStatus::ActivePendingRemoval)
		{
			ActiveTimers.pop();
			RemoveTimer(TopHandle);
			continue;
		}

		if (InternalTime > TopData.ExpireTime)
		{
			// remove it from the queue
			ActiveTimers.pop();
			TopData.Status = HTimerStatus::Executing;

			// determine how many times the timer may have elapsed (e.g. for large DeltaTime on a short looping timer)
			int32 CallCount = TopData.bLoop ? (InternalTime - TopData.ExpireTime) / TopData.Rate + 1 : 1;

			// now call the function
			for (int32 CallIndex = 0; CallIndex < CallCount; ++CallIndex)
			{
				TopData.TimerDelegate.Execute();

				// whether checking current top timer is invalidated
				if (TopData.Status != HTimerStatus::Executing)
				{
					break;
				}
			}

			// test to ensure it didn't get cleared during execution
			if (TopData.Status == HTimerStatus::Executing)
			{
				if (TopData.bLoop && TopData.TimerDelegate.IsValid())
				{
					// put this timer back on the heap
					TopData.ExpireTime += CallCount * TopData.Rate;
					TopData.Status = HTimerStatus::Active;
					ActiveTimers.push(TopData.Handle);
				}
				else
				{
					RemoveTimer(TopData.Handle);
				}
			}
		}
		else
		{
			break;
		}
	}

	// if we have any pending timers, add them to the active queue
	if (PendingTimers.size() > 0)
	{
		for (auto& PendingTimer : PendingTimers)
		{
			HTimerData& TimerToActivate = GetTimer(PendingTimer);

			// convert from time remaining back to a valid ExpireTime
			TimerToActivate.ExpireTime += InternalTime;
			TimerToActivate.Status = HTimerStatus::Active;
			ActiveTimers.push(PendingTimer);
		}

		PendingTimers.clear();
	}
}