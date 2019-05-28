#include "HMemoryPCH.h"
#include "HMemoryUtil.h"

uint32 HMemoryUtil::AlignedSize32(uint32 InSize, uint32 InAlignment)
{
	return (InSize + InAlignment) & (~(InAlignment - 1));
}

bool HMemoryUtil::IsAligned32(uint32 InValue, uint32 InAlignment)
{
	return InValue & (InAlignment - 1) == 0;
}

void* HMemoryUtil::AlignedPointer(void* InAddress, size_t InAlignment)
{
	return (void*)((size_t)InAddress & ~(InAlignment - 1));
}