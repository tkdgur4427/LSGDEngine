#pragma once

namespace lsgd {

	enum HTimerStatus
	{
		Pending,
		Active,
		Executing,
		ActivePendingRemoval,
	};

	// simple interface to wrap delegate that can be either native or dynamic
	struct HTimerUnifiedDelegate
	{
		HFunctionObject<void(void)> FuncCallback;

		void Execute()
		{
			if (FuncCallback)
			{
				FuncCallback();
			}
		}

		bool IsValid() const
		{
			return FuncCallback != nullptr;
		}
	};

	// unique handle that can be used to distinguish timers that have identical delegates
	struct HTimerHandle
	{
		HTimerHandle()
			: Handle(-1)
		{}

		static const uint32 IndexBits = 24;
		static const uint32 SerialNumberBits = 40;

		static const uint32 MaxIndex = (int32)1 << IndexBits;
		static const uint64 MaxSerialNumber = (int64)1 << SerialNumberBits;

		// helper methods
		void SetIndexAndSerialNumber(uint32 InIndex, uint64 InSerialNumber)
		{
			check(InSerialNumber < MaxSerialNumber);
			check(InIndex < MaxIndex);

			uint64 NewHandle = InSerialNumber << IndexBits;
			NewHandle |= (InIndex);
			
			Handle = NewHandle;
		}

		uint32 GetIndex() const
		{
			uint32 Index = Handle & ((uint64)MaxIndex - 1);
			return Index;
		}

		uint64 GetSerialNumber() const
		{
			uint64 SerialNumber = Handle >> IndexBits;
			return SerialNumber;
		}

		void Invalidate()
		{
			Handle = -1;
		}

		uint64 Handle;
	};

	struct HTimerData
	{
		uint8 bLoop : 1;
		
		// timer status
		HTimerStatus Status;

		// timer between set and fire, or repeat frequency if looping
		float Rate;

		double ExpireTime;

		// holds the delegate to call
		HTimerUnifiedDelegate TimerDelegate;

		// handle for this timer
		HTimerHandle Handle;
	};

	// timer handle predicate
	struct HTimerHandlePredicate
	{
		HTimerHandlePredicate(class HTimerManager* InTimerManager)
			: TimerManager(InTimerManager)
		{}

		bool operator()(HTimerHandle& TimerHandle0, HTimerHandle& TimerHandle1);

		class HTimerManager* TimerManager;
	};

	// class to globally manage timers
	class HTimerManager
	{
	public:
		HTimerManager()
			: ActiveTimers(HTimerHandlePredicate(this))
			, InternalTime(0.0)
		{}

		HTimerData& GetTimer(const HTimerHandle& InTimerHandle)
		{
			return Timers[InTimerHandle.GetIndex()];
		}

		void RemoveTimer(const HTimerHandle& InTimerHandle)
		{
			// add free index to the free list
			FreeList.push_back(InTimerHandle.GetIndex());
		}

		void ClearTimer(const HTimerHandle& InTimerHandle)
		{
			HTimerData& Data = GetTimer(InTimerHandle);
			switch (Data.Status)
			{
				case HTimerStatus::Pending:
				{
					PendingTimers.erase(PendingTimers.begin() + InTimerHandle.GetIndex());
					RemoveTimer(InTimerHandle);
				}
				break;

				case HTimerStatus::Active:
					Data.Status = HTimerStatus::ActivePendingRemoval;
					break;

				case HTimerStatus::ActivePendingRemoval:
					break;

				case HTimerStatus::Executing:
					RemoveTimer(InTimerHandle);
					break;

				default:
					check(false);
			}
		}

		HTimerHandle GenerateHandle(int32 Index)
		{
			uint64 NewSerialNumber = ++LastAssignedSerialNumber;
			if (NewSerialNumber >= HTimerHandle::MaxSerialNumber)
			{
				check(false);
			}

			HTimerHandle Result;
			Result.SetIndexAndSerialNumber(Index, NewSerialNumber);

			return Result;
		}

		HTimerHandle AddTimer(const HTimerData& TimerData)
		{
			int32 NewIndex = -1;
			if (FreeList.size() != 0)
			{
				NewIndex = FreeList.back();
				FreeList.pop_back();
			}
			else
			{
				NewIndex = Timers.size();
				Timers.push_back(TimerData);
			}			

			HTimerHandle Result = GenerateHandle(NewIndex);
			Timers[NewIndex].Handle = Result;

			return Result;
		}

		void SetTimer(HTimerHandle& InOutHandle, const HTimerUnifiedDelegate& InDelegate, float InRate, bool InbLoop, float InFirstDelay)
		{
			check(InOutHandle.Handle == -1); // not allowed clear timer for existing handle

			if (InRate > 0.f)
			{
				// set up the new timer
				HTimerData NewTimerData;
				NewTimerData.TimerDelegate = InDelegate;

				NewTimerData.Rate = InRate;
				NewTimerData.bLoop = InbLoop;
				
				const float FirstDelay = (InFirstDelay > 0.f) ? InFirstDelay : InRate;

				HTimerHandle NewTimerHandle;
				NewTimerData.ExpireTime = FirstDelay;
				NewTimerData.Status = HTimerStatus::Pending;
				
				NewTimerHandle = AddTimer(NewTimerData);
				PendingTimers.push_back(NewTimerHandle);

				InOutHandle = NewTimerHandle;
			}
			else
			{
				InOutHandle.Invalidate();
			}
		}

		void Tick(float DeltaTime);

		// free list for Timers
		HArray<int32> FreeList;

		// the array of timers
		HArray<HTimerData> Timers;
		
		// active queue
		HSortedArray<HTimerHandle, HTimerHandlePredicate> ActiveTimers;

		// pending queue
		HArray<HTimerHandle> PendingTimers;

		double InternalTime;

		static int64 LastAssignedSerialNumber;
	};
}