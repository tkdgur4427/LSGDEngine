#include "HMalloc.h"

namespace allocators {

	class HBaseAllocator;

}

using namespace allocators;

/*
	this is first initial version of binned malloc imitating UE4
		- it could be really just naive implementation

	TODO
		- memory address hash tables should be implemented like in UE4
		- MT supported binned malloc (free list for each bin)
*/

class HBinnedMalloc : public HMalloc
{
public:
	enum 
	{
		BINNED_MINIMUM_ALIGNMENT_SHIFT = 4,
		BINNED_MINIMUM_ALIGNMENT = 16,
		BINNED_MAX_SMALL_POOL_SIZE = 28672, // maximum small block size
		BINNED_SMALL_POOL_COUNT = 49,
		OS_MEMORY_PAGE_SIZE = 64 * 1024,
	};

	struct HBinnedMallocHeader
	{
		uint32 DirtyFlag;	// dirty flags checking
		uint32 Padding;		// note that this padding is value subtracting alloc address and aligned address
		uint32 Size;
		uint8 BinIndex;
	};

	HBinnedMalloc();
	~HBinnedMalloc();

	void Initialize();

	// utility methods
	uint32 GetBinIndexFromMemorySize(size_t InAlignedSize);
	uint32 GetMaximumBlockSize() const { return BINNED_MAX_SMALL_POOL_SIZE; }

	// memory alloc/dealloc methods
	virtual void* Malloc(size_t Count, uint32 Alignment = 16) override;
	virtual void* Realloc(void* Original, size_t Count, uint32 Alignment = 16) override;
	virtual bool Free(void* Original) override;

	// request memory size to binned memory index
	static uint8 MemorySizeToIndex[BINNED_MAX_SMALL_POOL_SIZE >> BINNED_MINIMUM_ALIGNMENT_SHIFT];

	// base allocator
	static HBaseAllocator* BlockAllocators[BINNED_SMALL_POOL_COUNT];
	static void InitializeBlockAllocators();

	// lock
	memory::synchronize::HCriticalSection CriticalSection;
};