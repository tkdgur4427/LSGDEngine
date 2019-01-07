#pragma once

namespace lsgd
{
	/*
		simple implementation for handler container
			- there are bunch of possible optimizations to apply
			- temporary
	*/

	template <class InterfaceType, class BusIdType = uint32>
	class HEHandlerContainer
	{
	public:
		HEHandlerContainer(uint32 InBusId)
			: BusId(InBusId)
		{}

		~HEHandlerContainer() {}
			
		using EBusIdType = BusIdType;
		using EHandlerType = InterfaceType;
		using EHandlerNode = shared_ptr<EHandlerType>;
		using Iterator = EHandlerNode*;

		void Register(EHandlerNode& InHandler)
		{
			Handlers.push_back(InHandler);
		}

		void Unregister(EHandlerNode& InHandler)
		{
			int32 Index = Find(InHandler);
			check(Index != -1);

			// unregister the handler
			Handlers.erase(Handlers.begin() + Index);
		}

		int32 Find(const EHandlerNode& InHandler)
		{
			int32 Result = -1;
			int32 TrackedIndex = 0;
			for (auto& Handler : Handlers)
			{
				if (Handler.get() == InHandler.get())
				{
					Result = TrackedIndex;
					break;
				}
				TrackedIndex++;
			}
			return Result;
		}

		Iterator Begin() { return Handlers.begin(); }
		Iterator End() { return Handlers.end(); }

		// assigned bus id
		BusIdType BusId;

		// handler array
		HArray<EHandlerNode> Handlers;
	};
}
