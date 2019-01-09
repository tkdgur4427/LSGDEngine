#pragma once

#include "..\Core\HConcurrentQueue.h"

using namespace lsgd::container;

namespace lsgd {
	
	template <class BusType>
	class HEBusCallQueue
	{
	public:
		using BusEventType = HFunctionObject<void()>;
		using BusEventQueueType = HConcurrentQueue<BusEventType>;

		BusEventQueueType CallQueue;

		template <class FunctionType, class... ArgTypes>
		void QueueEvent(FunctionType Function, ArgTypes&&... Args)
		{
			// note that I use variadic argument captures with lambda
			CallQueue.Push([Function, Args...](){
				Function(Args...); // call function
			});
		}

		template <class FunctionType, class... ArgTypes>
		void QueueBroadcastEvent(FunctionType Function, ArgTypes&&... Args)
		{
			CallQueue.Push([Function, Args...](){
				BusType::Broadcast(Function, Args...);
			});
		}

		void Execute()
		{
			// execute queued bus events
			while (CallQueue.Num() > 0)
			{
				// pop and execute each event
				BusEventType BusEvent = CallQueue.Pop();
				BusEvent();
			}
		}
	};

}