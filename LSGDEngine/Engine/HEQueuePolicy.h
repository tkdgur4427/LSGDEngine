#pragma once

namespace lsgd
{
	template <class ContextPolicy>
	class EQueuePolicy
	{
	public:
		using ContextType = typename ContextPolicy::ContextType;

		template <class FunctionType, class... ArgTypes>
		static void QueueEvent(FunctionType Function, ArgTypes&&... Args)
		{
			ContextType& Context = ContextPolicy::GetGlobalContext();
			Context.BusCallQueue.QueueEvent(Function, Args...);
		}

		template <class FunctionType, class... ArgTypes>
		static void QueueBroadcastEvent(FunctionType Function, ArgTypes&&... Args)
		{
			ContextType& Context = ContextPolicy::GetGlobalContext();
			Context.BusCallQueue.QueueBroadcastEvent(Function, Args...);
		}

		static void ExecuteQueuedEvents()
		{
			ContextType& Context = ContextPolicy::GetGlobalContext();
			Context.BusCallQueue.Execute();
		}
	};
}