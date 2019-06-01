#include "HMemoryPCH.h"
#include "HBinnedMalloc.h"
#include "HBlockAllocator.h"

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

HBaseAllocator* HBinnedMalloc::BlockAllocators[] = { 0, };

void HBinnedMalloc::InitializeBlockAllocators()
{
	// @todo - just listed small block sizes for block allocators
	static HBlockAllocator<HBlockAllocatorPolicy<16, OS_MEMORY_PAGE_SIZE>> BlockAllocator16;
	static HBlockAllocator<HBlockAllocatorPolicy<32, OS_MEMORY_PAGE_SIZE>> BlockAllocator32;
	static HBlockAllocator<HBlockAllocatorPolicy<64, OS_MEMORY_PAGE_SIZE>> BlockAllocator64;
	static HBlockAllocator<HBlockAllocatorPolicy<80, OS_MEMORY_PAGE_SIZE>> BlockAllocator80;
	static HBlockAllocator<HBlockAllocatorPolicy<96, OS_MEMORY_PAGE_SIZE>> BlockAllocator96;
	static HBlockAllocator<HBlockAllocatorPolicy<112, OS_MEMORY_PAGE_SIZE>> BlockAllocator112;
	static HBlockAllocator<HBlockAllocatorPolicy<128, OS_MEMORY_PAGE_SIZE>> BlockAllocator128;

	static HBlockAllocator<HBlockAllocatorPolicy<160, OS_MEMORY_PAGE_SIZE>> BlockAllocator160;
	static HBlockAllocator<HBlockAllocatorPolicy<192, OS_MEMORY_PAGE_SIZE>> BlockAllocator192;
	static HBlockAllocator<HBlockAllocatorPolicy<224, OS_MEMORY_PAGE_SIZE>> BlockAllocator224;
	static HBlockAllocator<HBlockAllocatorPolicy<256, OS_MEMORY_PAGE_SIZE>> BlockAllocator256;
	static HBlockAllocator<HBlockAllocatorPolicy<320, OS_MEMORY_PAGE_SIZE>> BlockAllocator320;
	static HBlockAllocator<HBlockAllocatorPolicy<384, OS_MEMORY_PAGE_SIZE>> BlockAllocator384;
	static HBlockAllocator<HBlockAllocatorPolicy<448, OS_MEMORY_PAGE_SIZE>> BlockAllocator448;

	static HBlockAllocator<HBlockAllocatorPolicy<512, OS_MEMORY_PAGE_SIZE>> BlockAllocator512;
	static HBlockAllocator<HBlockAllocatorPolicy<576, OS_MEMORY_PAGE_SIZE>> BlockAllocator576;
	static HBlockAllocator<HBlockAllocatorPolicy<640, OS_MEMORY_PAGE_SIZE>> BlockAllocator640;
	static HBlockAllocator<HBlockAllocatorPolicy<704, OS_MEMORY_PAGE_SIZE>> BlockAllocator704;
	static HBlockAllocator<HBlockAllocatorPolicy<768, OS_MEMORY_PAGE_SIZE>> BlockAllocator768;
	static HBlockAllocator<HBlockAllocatorPolicy<896, OS_MEMORY_PAGE_SIZE>> BlockAllocator896;
	static HBlockAllocator<HBlockAllocatorPolicy<1008, OS_MEMORY_PAGE_SIZE>> BlockAllocator1008;
	static HBlockAllocator<HBlockAllocatorPolicy<1168, OS_MEMORY_PAGE_SIZE>> BlockAllocator1168;

	static HBlockAllocator<HBlockAllocatorPolicy<1360, OS_MEMORY_PAGE_SIZE>> BlockAllocator1360;
	static HBlockAllocator<HBlockAllocatorPolicy<1632, OS_MEMORY_PAGE_SIZE>> BlockAllocator1632;
	static HBlockAllocator<HBlockAllocatorPolicy<2032, OS_MEMORY_PAGE_SIZE>> BlockAllocator2032;
	static HBlockAllocator<HBlockAllocatorPolicy<2336, OS_MEMORY_PAGE_SIZE>> BlockAllocator2336;
	static HBlockAllocator<HBlockAllocatorPolicy<2720, OS_MEMORY_PAGE_SIZE>> BlockAllocator2720;
	static HBlockAllocator<HBlockAllocatorPolicy<3264, OS_MEMORY_PAGE_SIZE>> BlockAllocator3264;
	static HBlockAllocator<HBlockAllocatorPolicy<4080, OS_MEMORY_PAGE_SIZE>> BlockAllocator4080;
	static HBlockAllocator<HBlockAllocatorPolicy<4368, OS_MEMORY_PAGE_SIZE>> BlockAllocator4368;

	static HBlockAllocator<HBlockAllocatorPolicy<4672, OS_MEMORY_PAGE_SIZE>> BlockAllocator4672;
	static HBlockAllocator<HBlockAllocatorPolicy<5040, OS_MEMORY_PAGE_SIZE>> BlockAllocator5040;
	static HBlockAllocator<HBlockAllocatorPolicy<5456, OS_MEMORY_PAGE_SIZE>> BlockAllocator5456;
	static HBlockAllocator<HBlockAllocatorPolicy<5952, OS_MEMORY_PAGE_SIZE>> BlockAllocator5952;
	static HBlockAllocator<HBlockAllocatorPolicy<6528, OS_MEMORY_PAGE_SIZE>> BlockAllocator6528;
	static HBlockAllocator<HBlockAllocatorPolicy<7280, OS_MEMORY_PAGE_SIZE>> BlockAllocator7280;
	static HBlockAllocator<HBlockAllocatorPolicy<8176, OS_MEMORY_PAGE_SIZE>> BlockAllocator8176;
	static HBlockAllocator<HBlockAllocatorPolicy<9360, OS_MEMORY_PAGE_SIZE>> BlockAllocator9360;

	static HBlockAllocator<HBlockAllocatorPolicy<10912, OS_MEMORY_PAGE_SIZE>> BlockAllocator10912;
	static HBlockAllocator<HBlockAllocatorPolicy<13104, OS_MEMORY_PAGE_SIZE>> BlockAllocator13104;
	static HBlockAllocator<HBlockAllocatorPolicy<16368, OS_MEMORY_PAGE_SIZE>> BlockAllocator16368;
	static HBlockAllocator<HBlockAllocatorPolicy<32752, OS_MEMORY_PAGE_SIZE>> BlockAllocator32752;

	BlockAllocators[0] = &BlockAllocator16;
	BlockAllocators[1] = &BlockAllocator32;
	BlockAllocators[2] = &BlockAllocator64;
	BlockAllocators[3] = &BlockAllocator80;
	BlockAllocators[4] = &BlockAllocator96;
	BlockAllocators[5] = &BlockAllocator112;
	BlockAllocators[6] = &BlockAllocator128;

	BlockAllocators[7] = &BlockAllocator160;
	BlockAllocators[8] = &BlockAllocator192;
	BlockAllocators[9] = &BlockAllocator224;
	BlockAllocators[10] = &BlockAllocator256;
	BlockAllocators[11] = &BlockAllocator320;
	BlockAllocators[12] = &BlockAllocator384;
	BlockAllocators[13] = &BlockAllocator448;

	BlockAllocators[14] = &BlockAllocator512;
	BlockAllocators[15] = &BlockAllocator576;
	BlockAllocators[16] = &BlockAllocator640;
	BlockAllocators[17] = &BlockAllocator704;
	BlockAllocators[18] = &BlockAllocator768;
	BlockAllocators[19] = &BlockAllocator896;
	BlockAllocators[20] = &BlockAllocator1008;
	BlockAllocators[21] = &BlockAllocator1168;

	BlockAllocators[22] = &BlockAllocator1360;
	BlockAllocators[23] = &BlockAllocator1632;
	BlockAllocators[24] = &BlockAllocator2032;
	BlockAllocators[25] = &BlockAllocator2336;
	BlockAllocators[26] = &BlockAllocator2720;
	BlockAllocators[27] = &BlockAllocator3264;
	BlockAllocators[28] = &BlockAllocator4080;
	BlockAllocators[29] = &BlockAllocator4368;

	BlockAllocators[30] = &BlockAllocator4672;
	BlockAllocators[31] = &BlockAllocator5040;
	BlockAllocators[32] = &BlockAllocator5456;
	BlockAllocators[33] = &BlockAllocator5952;
	BlockAllocators[34] = &BlockAllocator6528;
	BlockAllocators[35] = &BlockAllocator7280;
	BlockAllocators[36] = &BlockAllocator8176;
	BlockAllocators[37] = &BlockAllocator9360;

	BlockAllocators[38] = &BlockAllocator10912;
	BlockAllocators[39] = &BlockAllocator13104;
	BlockAllocators[40] = &BlockAllocator16368;
	BlockAllocators[41] = &BlockAllocator32752;
}

HBinnedMalloc::HBinnedMalloc()
{
	// initialize block allocators
	HBinnedMalloc::InitializeBlockAllocators();

	// initialize
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
	uint32 HashIndex = (uint32)((InAlignedSize + (BINNED_MINIMUM_ALIGNMENT - 1)) >> BINNED_MINIMUM_ALIGNMENT_SHIFT);
	mcheck(HashIndex < BINNED_MAX_SMALL_POOL_SIZE >> BINNED_MINIMUM_ALIGNMENT_SHIFT);
	
	return (uint32)MemorySizeToIndex[HashIndex];
}

void* HBinnedMalloc::Malloc(size_t Count, uint32 Alignment)
{
	memory::synchronize::HScopedLock Lock(CriticalSection);

	mcheck(HMemoryUtil::IsAligned32(Alignment, BINNED_MINIMUM_ALIGNMENT));

	// total size includes HBinnedMallocHeader
	uint32 TotalSize = Count + sizeof(HBinnedMallocHeader);
	uint32 AlignedTotalSize = HMemoryUtil::AlignedSize32(TotalSize, Alignment);
	uint32 BinIndex = GetBinIndexFromMemorySize(AlignedTotalSize);

	uint8* AllocAddress = (uint8*)BlockAllocators[BinIndex]->Allocate(AlignedTotalSize);

	uint8* AllocAddressWithHeaderSize = AllocAddress + sizeof(HBinnedMallocHeader);	
	uint8* AlignedAddress = (uint8*)HMemoryUtil::AlignedUpPointer(AllocAddressWithHeaderSize, Alignment);

	// update the header value
	HBinnedMallocHeader* Header = (HBinnedMallocHeader*)(AlignedAddress - sizeof(HBinnedMallocHeader));
	Header->Padding = AlignedAddress - AllocAddress;
	Header->BinIndex = BinIndex;
	Header->Size = Count;
	Header->DirtyFlag = 0xDEADBABE;

	// return aligned address
	return AlignedAddress;
}

void* HBinnedMalloc::Realloc(void* Original, size_t Count, uint32 Alignment)
{
	memory::synchronize::HScopedLock Lock(CriticalSection);

	HBinnedMallocHeader* Header = (HBinnedMallocHeader*)((uint8*)Original - sizeof(HBinnedMallocHeader));

	void* NewAddress = Malloc(Count, Alignment);
	HGenericMemory::MemCopy(NewAddress, Original, Header->Size);
	Free(Original);

	return NewAddress;
}

bool HBinnedMalloc::Free(void* Original)
{
	memory::synchronize::HScopedLock Lock(CriticalSection);

	HBinnedMallocHeader* Header = (HBinnedMallocHeader*)((uint8*)Original - sizeof(HBinnedMallocHeader));
	if (Header->DirtyFlag != 0xDEADBABE)
	{
		return false;
	}

	uint32 BinIndex = Header->BinIndex;
	uint32 Padding = Header->Padding;
	uint32 BlockSize = SmallBlockSizes[BinIndex];

	void* AddressToFree = (uint8*)Original - Padding;
	BlockAllocators[BinIndex]->Deallocate(AddressToFree);

	return true;
}
