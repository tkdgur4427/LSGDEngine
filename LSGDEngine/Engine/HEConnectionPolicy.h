#pragma once

namespace lsgd
{
	template <class ContextPolicy>
	struct HEConnectionPolicy
	{
		using ContextType = typename ContextPolicy::ContextType;
		using BusIdType = typename ContextPolicy::BusIdType;
		using HandlerNodeType = typename ContextPolicy::HandlerNodeType;

		static void Bind(const BusIdType& InBusId)
		{
			ContextType& Context = ContextPolicy::GetGlobalContext();
			Context.BusContainer.Register(InBusId);
		}

		static void Unbind(const BusIdType& InBusId)
		{
			ContextType& Context = ContextPolicy::GetGlobalContext();
			Context.BusContainer.Unregister(InBusId);
		}

		static void Connect(const BusIdType& InBusId, HandlerNodeType& InHandler)
		{
			ContextType& Context = ContextPolicy::GetGlobalContext();
			Context.BusContainer.Buses[InBusId]->Register(InHandler);
		}

		static void DisConnect(const BusIdType& InBusId, HandlerNodeType& InHandler)
		{
			ContextType& Context = ContextPolicy::GetGlobalContext();
			Context.BusContainer.Buses[InBusId]->Unregister(InHandler);
		}
	};
}