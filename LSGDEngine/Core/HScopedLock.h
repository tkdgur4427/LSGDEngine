#pragma once

namespace lsgd { namespace thread {

	class HScopedLock
	{
	public:
		HScopedLock(HCriticalSection& InSyncObject)
			: SyncObject(InSyncObject)
		{
			SyncObject.lock();
		}

		~HScopedLock()
		{
			SyncObject.unlock();
		}

		HCriticalSection& SyncObject;
	};

} }