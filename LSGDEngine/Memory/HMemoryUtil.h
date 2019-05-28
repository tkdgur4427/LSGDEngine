#pragma once

// @todo - temporary assert functionalties
#define mcheck(condition) if(!(condition)) { *((int*)0) = 3; }

class HMemoryUtil
{
public:
	static uint32 AlignedSize32(uint32 InSize, uint32 InAlignment);
	static bool IsAligned32(uint32 InValue, uint32 InAlignment);
	static void* AlignedPointer(void* InAddress, size_t InAlignment);
};