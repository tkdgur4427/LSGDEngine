#pragma once

class HGenericMemory
{
public:
	// default alloc/dealloc
	static void* Allocate(int InSize, int InAlignment = 4);
	static void Deallocate(void* InPointer);
	
	// large alloc/dealloc (based on buddy allocator)
	static void* AllocateLarge(int InSize, int InAlignment = 4);
	static void DeallocateLarge(void* InPointer);
	
	// stack mem alloc (based on stack allocator)
	static void PushMemMark(char* InName = nullptr);
	static void PopMemMark();
	static void* AllocateMemStack(int InSize, int InAlignment = 4);
};