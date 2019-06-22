#include "HMemoryPCH.h"
#include "HGenericMemory.h"

#include "HBinnedMalloc.h"
#include "HStompMalloc.h"

#define USE_TEMP_ALLOC_DEALLOC 1

#define USE_BINNED_MALLOC 1
#define USE_STOMP_MALLOC 0

#if USE_BINNED_MALLOC

HStompMalloc& GetStompMalloc()
{
	static HStompMalloc GStompMalloc;
	return GStompMalloc;
}

HBinnedMalloc& GetBinnedMalloc()
{
	static HBinnedMalloc GBinnedMalloc;
	return GBinnedMalloc;
}

HMalloc* GetMalloc()
{
	static HMalloc* GMalloc = &GetBinnedMalloc();
#if USE_STOMP_MALLOC
	GMalloc = &GetStompMalloc();
#endif
	return GMalloc;
}
#endif

#if USE_TEMP_ALLOC_DEALLOC

// for temporary wrapper implementation
void* TempAllocate(size_t InSize)
{
#if USE_BINNED_MALLOC
	void* NewPointer = nullptr;
	if (InSize <= GetBinnedMalloc().GetMaximumBlockSize())
	{
		NewPointer = GetMalloc()->Malloc(InSize);
	}
	else
	{
		NewPointer = malloc(InSize);
	}
#else
	void* NewPointer = malloc(InSize);
#endif

	// clear the memory
	std::memset(NewPointer, 0, InSize);

	return NewPointer;
}

void TempDeallocate(void* InPointer)
{
#if USE_BINNED_MALLOC
	if (!GetMalloc()->Free(InPointer))
	{
		free(InPointer);
	}
#else
	free(InPointer);
#endif
}
#endif

void* HGenericMemory::Allocate(size_t InSize, size_t InAlignment)
{
#if USE_TEMP_ALLOC_DEALLOC
	return TempAllocate(InSize);
#endif
}

void HGenericMemory::Deallocate(void* InPointer)
{
#if USE_TEMP_ALLOC_DEALLOC
	return TempDeallocate(InPointer);
#endif
}

void* HGenericMemory::AllocateLarge(size_t InSize, size_t InAlignment)
{
#if USE_TEMP_ALLOC_DEALLOC
	return TempAllocate(InSize);
#endif
}

void HGenericMemory::DeallocateLarge(void* InPointer)
{
#if USE_TEMP_ALLOC_DEALLOC
	return TempDeallocate(InPointer);
#endif
}

#define TEMP_STACK_ALLOC 1
#if TEMP_STACK_ALLOC

#define TEMP_STACK_MEM_SIZE 2 * 1024 * 1024
char TempStackAlloc[TEMP_STACK_MEM_SIZE];
int StackOffset = 0;

#define STACK_MARK_MAX_DEPTH 1024
int MemMark[STACK_MARK_MAX_DEPTH] = { 0, };
int MemMarkOffset = 0;

#endif

void HGenericMemory::PushMemMark(char* InName)
{
#if TEMP_STACK_ALLOC
	MemMark[MemMarkOffset] = StackOffset;
	MemMarkOffset++;
#endif
}

void HGenericMemory::PopMemMark()
{
#if TEMP_STACK_ALLOC
	StackOffset = MemMark[MemMarkOffset - 1];
	MemMarkOffset--;
#endif
}

void* HGenericMemory::AllocateMemStack(int InSize, int InAlignment)
{
#if TEMP_STACK_ALLOC
	void* Memory = (void*)&TempStackAlloc[StackOffset];
	StackOffset += InSize;

	return Memory;
#endif
}

void HGenericMemory::MemZero(void* Dest, int InValue, size_t InSize)
{
	memset(Dest, InValue, InSize);
}

void* HGenericMemory::MemCopy(void* Dest, void const* Src, size_t InSize)
{
	return memcpy(Dest, Src, InSize);
}