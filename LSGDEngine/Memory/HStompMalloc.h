#pragma once

#include "HMalloc.h"

/*
	stomp memory allocator; it helps find the following errors:
		- R/W off the end of an allocation
		- R/W off the beginning of an allocation
		- R/W after freeing an allocation
*/

class HStompMalloc : public HMalloc
{
public:
	static const int64 SentinelExpectedValue = 0xdeadbeafdeadbeef;
	static const bool bUseUnderrunMode = false;
	
	enum
	{
		PageSize = 4 * 1024, // in windows, 4KB is page size for CPU memory
	};

	struct HAllocationData
	{
		// pointer to the full allocation; needed so the OS knows what to free
		void* FullAllocationPointer;
		// full size of allocation including extra page
		int64 FullSize;
		// size of the allocation requested
		int64 Size;
		// sentinel used to check for underrun
		int64 Sentinel;
	};

	uint8* VirtualAddressCursor = nullptr;
	int64 VirtualAddressMax = 0;
	static const int64 VirtualAddressBlockSize = 1 * 1024 * 1024 * 1024; // 1GB blocks

	// synchronization
	memory::synchronize::HCriticalSection CriticalSection;

	HStompMalloc() {}
	virtual ~HStompMalloc() {}

	/*
		allocates a block of a given number of bytes of memory with the required alignment
		in the process it allocates as many pages as necessary plus on that will be protected
		making it unaccessible and causing an exception
			- the actual allocation will be pushed to the end of the last valid unprotected page
			- to deal with underrun errors a sentinel is added right before the allocation in page which is checked on free
	*/
	virtual void* Malloc(size_t Count, uint32 Alignment = 16) override;
	virtual void* Realloc(void* Original, size_t Count, uint32 Alignment = 16) override;
	virtual bool Free(void* Original) override;
};