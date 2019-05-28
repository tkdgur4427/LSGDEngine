#include "HMemoryPCH.h"
#include "HBinnedMalloc.h"

// small memory block sizes
//	- note that it is UE4 reference implementations
static uint32 SmallBlockSizes[] = {
	16, 32, 64, 80, 96, 112, 128,
	160, 192, 224, 256, 320, 384, 448,
	512, 576, 640, 704, 768, 896, 1024 - 16, 1168,
	1360, 1632, 2048 - 16, 2336, 2720, 3264, 4096 - 16, 4368,
	4672, 5040, 5456, 5952, 6544 - 16, 7280, 8192 - 16, 9360,
	10912, 13104, 16384 - 16, 32768 - 16
};

// static class variable definitions
uint8 HBinnedMalloc::MemorySizeToIndex[] = { 0, };

HBinnedMalloc::HBinnedMalloc()
{
	Initialize();
}

HBinnedMalloc::~HBinnedMalloc()
{

}

void HBinnedMalloc::Initialize()
{
	// if it larger than BINNED_MINIMUM_ALIGNMENT, we should consider some modification for this
	mcheck(sizeof(HBinnedMallocHeader) < BINNED_MINIMUM_ALIGNMENT);

	// construct memory size to pool index mapper (for fast access)
	uint8* IndexEntry = MemorySizeToIndex;
	uint32 PoolIndex = 0;
	for (int32 Index = 0; Index != 1 + (BINNED_MAX_SMALL_POOL_SIZE >> BINNED_MINIMUM_ALIGNMENT_SHIFT); ++Index)
	{
		uint32 BlockSize = Index << BINNED_MINIMUM_ALIGNMENT_SHIFT;

		while (SmallBlockSizes[PoolIndex] < BlockSize)
		{
			++PoolIndex;
		}

		*(IndexEntry++) = PoolIndex;
	}
}

uint32 HBinnedMalloc::GetBinIndexFromMemorySize(size_t InAlignedSize)
{
	uint32 HashIndex = (uint32)((InAlignedSize + (BINNED_MINIMUM_ALIGNMENT - 1)) >> BINNED_MINIMUM_ALIGNMENT_SHIFT);
	mcheck(HashIndex < BINNED_MAX_SMALL_POOL_SIZE >> BINNED_MINIMUM_ALIGNMENT_SHIFT);
	
	return (uint32)MemorySizeToIndex[HashIndex];
}

void* HBinnedMalloc::Malloc(size_t Count, unsigned int Alignment)
{
	mcheck(HMemoryUtil::IsAligned32(Alignment, BINNED_MINIMUM_ALIGNMENT));

	uint32 TotalSize = HMemoryUtil::AlignedSize32(Count, Alignment);
	uint32 BinIndex = GetBinIndexFromMemorySize(TotalSize);
	uint32 BlockSize = SmallBlockSizes[BinIndex];

	unsigned char* AllocAddress = (unsigned char*)HBinnedMalloc::GetBinnedAllocator<BlockSize>()->AllocateMemoryBlock();
	unsigned char* AlignedAddress = (unsigned char*)HMemoryUtil::AlignedPointer(AllocAddress, Alignment);

	// update the header value
	HBinnedMallocHeader* Header = (HBinnedMallocHeader*)(AlignedAddress - sizeof(HBinnedMallocHeader));
	Header->Padding = AlignedAddress - AllocAddress;
	Header->BinIndex = BinIndex;

	// return aligned address
	return AlignedAddress;
}

void* HBinnedMalloc::Realloc(void* Original, size_t Count, unsigned int Alignment)
{
	return nullptr;
}

void HBinnedMalloc::Free(void* Original)
{
	HBinnedMallocHeader* Header = (HBinnedMallocHeader*)((unsigned char*)Original - sizeof(HBinnedMallocHeader));
	
	uint32 BinIndex = Header->BinIndex;
	uint32 Padding = Header->Padding;
	uint32 BlockSize = SmallBlockSizes[BinIndex];

	void* AddressToFree = (unsigned char*)Original - Padding;
	HBinnedMalloc::GetBinnedAllocator<BlockSize>()->DeallocateMemoryBlock(AddressToFree);
}