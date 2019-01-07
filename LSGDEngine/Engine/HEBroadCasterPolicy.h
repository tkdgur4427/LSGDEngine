#pragma once

namespace lsgd
{
	template <typename ContextPolicy>
	class HEBroadCasterPolicy
	{
	public:		
		using ContextType = typename ContextPolicy::ContextType;
		using BusIdType = typename ContextPolicy::BusIdType;
		using HandlerNodeType = typename ContextPolicy::HandlerNodeType;

		template <class FunctionType, class... ArgTypes>
		static void Broadcast(FunctionType Function, ArgTypes&&... Args)
		{
			ContextType& Context = ContextPolicy::GetGlobalContext();
			
			if (Context.BusContainer.Num() > 0)
			{
				auto& Start = Context.BusContainer.Begin();
				auto& End = Context.BusContainer.End();

				// looping buses
				auto CurrBus = Start;
				while (CurrBus != End)
				{
					auto& StartHandler = CurrBus.Begin();
					auto& EndHandler = CurrBus.End();

					// looping handlers and trigger following Function(FunctionType)
					auto CurrHandler = StartHandler;					
					while (CurrHandler != EndHandler)
					{
						(CurrHandler->*Function)(Args...); // call function
						CurrHandler++;
					}

					CurrBus++;
				}
			}
		}
	};
}