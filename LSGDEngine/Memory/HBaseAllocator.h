#pragma once

namespace allocators {

	class HBaseAllocator
	{
	public:
		HBaseAllocator() {}
		~HBaseAllocator() {}

		virtual void* Allocate(size_t InSize) = 0;
		virtual void Deallocate(void* InPointer) = 0;
	};

}