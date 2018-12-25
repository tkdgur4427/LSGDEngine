#pragma once

namespace lsgd { namespace thread {

	class HScopedLock
	{
	public:
		HScopedLock(HCriticalSection& InSyncObject)
			: SyncObject(InSyncObject)
		{
			SyncObject.Lock();
		}

		~HScopedLock()
		{
			SyncObject.Unlock();
		}

		HCriticalSection& SyncObject;
	};

} }