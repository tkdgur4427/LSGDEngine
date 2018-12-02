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
		HConcurrentQueue() {}
		virtual ~HConcurrentQueue() {}

		void Push(const Type& InType)
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

		uint32 Num() { HScopedLock Lock(SyncObject); return Queue.size(); }

	private:
		HCriticalSection SyncObject;
		HQueue<Type> Queue;
	};

	/*
		Closable?
			- usually this structure is useful to reusing any instance having ConcurrentQueue
			- or naive ConcurrentQueue; we could reduce any memory reallocation for every instance to create this queue
	*/

	template <class Type>
	class HClosableConcurrentQueue : public HConcurrentQueue<Type>
	{
	public:
		HClosableConcurrentQueue(bool InClosed = false)
			: Closed(InClosed)
		{}

		bool Push(const Type& InType)
		{
			if (Closed)
			{
				return false;
			}

			HConcurrentQueue<Type>::Push(InType);
			return true;
		}

		bool Pop(Type& OutValue)
		{
			if (Closed)
			{
				return false;
			}

			OutValue = HConcurrentQueue<Type>::Pop();
			return true;
		}

		uint32 Num()
		{
			if (Closed)
			{
				return -1;
			}

			return HConcurrentQueue<Type>::Num();
		}

		void SetClosed(bool InValue) { Closed = InValue; }
		bool IsClosed() const { return Closed; }

	protected:
		// whether concurrent queue is already closed or not
		HAtomic<bool> Closed;
	};

} }
