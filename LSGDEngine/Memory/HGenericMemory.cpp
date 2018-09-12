#include "HMemoryPCH.h"
#include "HGenericMemory.h"

#define USE_TEMP_ALLOC_DEALLOC 1

#if USE_TEMP_ALLOC_DEALLOC

// for temporary wrapper implementation
void* TempAllocate(int InSize)
{
	void* NewPointer = malloc(InSize);

	// clear the memory
	std::memset(NewPointer, 0, InSize);

	return NewPointer;
}

void TempDeallocate(void* InPointer)
{
	free(InPointer);
}
#endif

void* HGenericMemory::Allocate(int InSize, int InAlignment)
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

void* HGenericMemory::AllocateLarge(int InSize, int InAlignment)
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