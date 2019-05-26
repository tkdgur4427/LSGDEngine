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
	uint32 HashIndex = (uint32)((InAlignedSize + BINNED_MINIMUM_ALIGNMENT) >> BINNED_MINIMUM_ALIGNMENT_SHIFT);
	return (uint32)MemorySizeToIndex[HashIndex];
}

void* HBinnedMalloc::Malloc(size_t Count, unsigned int Alignment)
{
	uint32 TotalSize = Count + Alignment;
	return nullptr;
}

void* HBinnedMalloc::Realloc(void* Original, size_t Count, unsigned int Alignment)
{
	return nullptr;
}

void HBinnedMalloc::Free(void* Original)
{

}