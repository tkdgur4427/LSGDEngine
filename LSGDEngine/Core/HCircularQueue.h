#pragma once

namespace lsgd { namespace container {

	// template for circular buffer
	template <typename ElementType>
	class HCircularBuffer
	{
	public:
		explicit HCircularBuffer(uint32 Capacity)
		{
			Elements.resize(Capacity);
			IndexMask = Elements.size() - 1;
		}

		ElementType& operator[](uint32 Index)
		{
			return Elements[Index & IndexMask];
		}

		const ElementType& operator[](uint32 Index) const
		{
			return Elements[Index & IndexMask];
		}

		uint32 Capacity() const
		{
			return Elements.size();
		}

		uint32 GetNextIndex(uint32 CurrentIndex) const
		{
			return ((CurrentIndex + 1) & IndexMask);
		}

		uint32 GetPrevIndex(uint32 CurrentIndex) const
		{
			return ((CurrentIndex - 1) & IndexMask);
		}

		// holds the mask for indexing the buffer's elements
		uint32 IndexMask;
		// holds the buffer's elements
		HArray<ElementType> Elements;
	};

	/*
		implments a lock-free first-in, first-out queue using a circular array
		this class is thread safe only in single-producer single-consumer scenario

		the number of items that can be enqueued is one less than the queue's capacity, because one item will be used for detecting full and empty states
		
		there is some room for optimization via using fine grained memory fences, but the implication for all out target platform need further analysis
		so we're using the simpler sequential consistent model for now
	*/
	template <typename ElementType>
	class HCircularQueue
	{
	public:
		explicit HCircularQueue(uint32 CapacityPlusOne)
			: Buffer(CapacityPlusOne)
			, Head(0), Tail(0)
		{}

		uint32 Count() const
		{
			int32 Count = Tail.load() - Head.load();
			if (Count < 0)
			{
				Count += Buffer.Capacity();
			}
			return (uint32)Count;
		}

		bool Dequeue(ElementType& OutElement)
		{
			const uint32 CurrentHead = Head.load();
			if (CurrentHead != Tail.load())
			{
				OutElement = HMove(Buffer[CurrentHead]);
				Head.store(Buffer.GetNextIndex(CurrentHead));

				return true;
			}
			return false;
		}

		void Empty()
		{
			Head.store(Tail.load());
		}

		bool Enqueue(const ElementType& Element)
		{
			const uint32 CurrentTail = Tail.load();
			uint32 NewTail = Buffer.GetNextIndex(CurrentTail);

			if (NewTail != Head.load())
			{
				Buffer[CurrentTail] = Element;
				Tail.store(NewTail);
				return true;
			}
			return false;
		}

		bool IsEmpty() const
		{
			return (Head.load() == Tail.Load());
		}

		bool IsFull() const
		{
			return (Buffer.GetNextIndex(Tail.load()) == Head.load());
		}

		bool Peek(ElementType& OutItem)
		{
			const uint32 CurrentHead = Head.load();
			if (CurrentHead != Tail.load())
			{
				OutItem = Buffer[CurrentHead];
				return true;
			}
			return false;
		}

		// holds the buffer
		HCircularBuffer<ElementType> Buffer;
		// holds the index to the first/last item in the buffer
		HAtomic<uint32> Head, Tail;
	};
} }