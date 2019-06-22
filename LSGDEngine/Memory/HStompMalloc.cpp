#include "HMemoryPCH.h"
#include "HStompMalloc.h"

void* HStompMalloc::Malloc(size_t Count, uint32 Alignment)
{
	memory::synchronize::HScopedLock Lock(CriticalSection);

	if (Count == 0U)
	{
		Count = 1U;
	}

	// 64-bit ABIs on x86_64 expect a 16-byte alignment
	Alignment = Alignment > 16 ? Alignment : 16;

	const int64 AlignedSize = (Alignment > 0) ? ((Count + Alignment - 1) & -(int32)Alignment) : Count;
	const int64 AllocFullPageSize = AlignedSize + sizeof(HAllocationData) + (PageSize - 1) & ~(PageSize - 1);
	const int64 TotalAllocationSize = AllocFullPageSize + PageSize;

	// allocate virtual space from current block using linear allocation strategy
	// if there is not enough space, try to allocate new block from OS, and allocation fails
	void* FullAllocationPointer = nullptr;
	if ((int64)VirtualAddressCursor + TotalAllocationSize <= VirtualAddressMax)
	{
		FullAllocationPointer = (void*)VirtualAddressCursor;
	}
	else
	{
		const int64 ReserveSize = VirtualAddressBlockSize < TotalAllocationSize ? TotalAllocationSize : VirtualAddressBlockSize;

		// reserve a new block of virtual address space that will be linearly sub-allocated
		// we intentionally don't keep track of reserved blocks, as we never need to explicitly release them
		FullAllocationPointer = VirtualAlloc(nullptr, ReserveSize, MEM_RESERVE, PAGE_NOACCESS);

		VirtualAddressCursor = (uint8*)FullAllocationPointer;
		VirtualAddressMax = (int64)VirtualAddressCursor + ReserveSize;
	}

	// no atomics or locks required here, as Malloc is externally synchronized
	VirtualAddressCursor += TotalAllocationSize;

	mcheck(FullAllocationPointer != nullptr);

	void* ReturnedPointer = nullptr;
	int64 AllocationDataSize = sizeof(HAllocationData);

	const HAllocationData AllocData = { FullAllocationPointer, TotalAllocationSize, AlignedSize, SentinelExpectedValue };

	if (bUseUnderrunMode)
	{
		const int64 AlignedAllocationData = (Alignment > 0) ? ((AllocationDataSize + Alignment - 1) & -(int32)Alignment) : AllocationDataSize;
		ReturnedPointer = (HAllocationData*)((uint8*)FullAllocationPointer + PageSize + AlignedAllocationData);
		void* AllocDataPointerStart = (HAllocationData*)((uint8*)FullAllocationPointer + PageSize);

		// commit physical pages to the used range, leaving the first page unmapped
		void* CommittedMemory = VirtualAlloc(AllocDataPointerStart, AllocFullPageSize, MEM_COMMIT, PAGE_READWRITE);
		mcheck(CommittedMemory != nullptr);
	}
	else // overrun
	{
		ReturnedPointer = (void*)((uint8*)FullAllocationPointer + AllocFullPageSize - AlignedSize);

		void* CommittedMemory = VirtualAlloc(FullAllocationPointer, AllocFullPageSize, MEM_COMMIT, PAGE_READWRITE);
		mcheck(CommittedMemory != nullptr);
	}

	HAllocationData* AllocDataPointer = (HAllocationData*)((uint8*)ReturnedPointer - AllocationDataSize);
	*AllocDataPointer = AllocData;

	return ReturnedPointer;
}

void* HStompMalloc::Realloc(void* Original, size_t Count, uint32 Alignment)
{
	memory::synchronize::HScopedLock Lock(CriticalSection);

	if (Count == 0)
	{
		Free(Original);
		return nullptr;
	}

	void* ReturnPtr = nullptr;
	if (Original != nullptr)
	{
		ReturnPtr = Malloc(Count, Alignment);

		HAllocationData* AllocDataPtr = (HAllocationData*)((uint8*)Original - sizeof(HAllocationData));
		HGenericMemory::MemCopy(ReturnPtr, Original, AllocDataPtr->Size < Count ? AllocDataPtr->Size : Count);
		Free(Original);
	}
	else
	{
		ReturnPtr = Malloc(Count, Alignment);
	}

	return ReturnPtr;
}

bool HStompMalloc::Free(void* Original)
{
	memory::synchronize::HScopedLock Lock(CriticalSection);

	if (Original == nullptr)
	{
		return false;
	}

	HAllocationData* AllocDataPtr = (HAllocationData*)(Original);
	AllocDataPtr--;

	// something underrun happens!
	mcheck(AllocDataPtr->Sentinel == SentinelExpectedValue);

	VirtualFree(AllocDataPtr->FullAllocationPointer, AllocDataPtr->FullSize, MEM_DECOMMIT);

	return true;
}