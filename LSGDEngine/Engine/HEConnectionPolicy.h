#pragma once

namespace lsgd
{
	template <class ContextPolicy>
	struct HEConnectionPolicy
	{
		using ContextType = typename ContextPolicy::ContextType;
		using BusIdType = typename ContextPolicy::BusIdType;
		using HandlerNodeType = typename ContextPolicy::HandlerNodeType;

		static bool IsBusExist(const BusIdType& InBusId)
		{
			ContextType& Context = ContextPolicy::GetGlobalContext();
			return Context.Exist(InBusId);
		}

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
			check(Context.Exist(InBusId));
			Context.BusContainer.Buses[InBusId]->Register(InHandler);
		}

		static void Disconnect(const BusIdType& InBusId, HandlerNodeType& InHandler)
		{
			ContextType& Context = ContextPolicy::GetGlobalContext();
			check(Context.Exist(InBusId));
			Context.BusContainer.Buses[InBusId]->Unregister(InHandler);
		}

		// additional helper method
		static void BindAndConnect(const BusIdType& InBusId, HandlerNodeType& InHandler)
		{	
			if (!IsBusExist(InBusId)) // if bus is not binded, bind first
			{
				Bind(InBusId);
			}				
			Connect(InBusId, InHandler);
		}
	};
}