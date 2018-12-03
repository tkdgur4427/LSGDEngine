#pragma once

namespace lsgd { namespace thread {

	class HThreadSafeCounter
	{
	public:
		HThreadSafeCounter()
			: Counter(0)
		{}
		HThreadSafeCounter(uint32 InValue)
			: Counter(InValue)
		{}

		~HThreadSafeCounter() {}

		// increment/decrement returns previous value of Counter
		uint32 Increment() { return Counter++; }
		uint32 Decrement() { return Counter--; }
		uint32 GetValue() { return Counter; }
		void SetValue(uint32 InValue) { Counter = InValue; }

	protected:
		HAtomic<uint32> Counter;
	};	
} }