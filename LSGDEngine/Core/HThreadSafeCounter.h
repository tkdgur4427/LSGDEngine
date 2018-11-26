#pragma once

namespace lsgd { namespace thread {

	class HThreadSafeCounter
	{
	public:
		HThreadSafeCounter()
			: Counter(0)
		{

		}

		~HThreadSafeCounter() {}

		// increment/decrement returns previous value of Counter
		uint32 Increment() { return Counter++; }
		uint32 Decrement() { return Counter--; }

	protected:
		HAtomic<uint32> Counter;
	};

} }