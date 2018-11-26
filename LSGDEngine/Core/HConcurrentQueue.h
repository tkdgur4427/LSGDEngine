#pragma once

using namespace lsgd::thread;

namespace lsgd { namespace container {

	/*
		@todo - need to make it lock-free queue
		for now, temporary just implement thread-safe queue
	*/

	template <class Type>
	class HConcurrentQueue
	{
	public:
		void Push(Type&& InType)
		{
			HScopedLock Lock(SyncObject);
			Queue.push(InType);
		}

		Type Pop()
		{
			HScopedLock Lock(SyncObject);

			Type Result = Queue.back();
			Queue.pop();

			return Result;
		}

		uint32 Num() const { HScopedLock Lock(SyncObject); return Queue.size(); }

	private:
		HCriticalSection SyncObject;
		HQueue<Type> Queue;
	};

} }
