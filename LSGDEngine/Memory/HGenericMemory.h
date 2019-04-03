#pragma once

class HGenericMemory
{
public:
	template <class Type, typename ...Arguments>
	static Type* AllocateObject(Arguments&&... InArguments)
	{
		void* NewObject = (void*)Allocate(sizeof(Type));
		return new (NewObject) Type(std::forward<Arguments>(InArguments)...);
	}

	// default alloc/dealloc
	static void* Allocate(size_t InSize, size_t InAlignment = 4);
	static void Deallocate(void* InPointer);
	
	// large alloc/dealloc (based on buddy allocator)
	static void* AllocateLarge(size_t InSize, size_t InAlignment = 4);
	static void DeallocateLarge(void* InPointer);
	
	// stack mem alloc (based on stack allocator)
	static void PushMemMark(char* InName = nullptr);
	static void PopMemMark();
	static void* AllocateMemStack(int InSize, int InAlignment = 4);

	// memory operations
	static void MemZero(void* Dest, int InValue, size_t InSize);
	static void* MemCopy(void* Dest, void const* Src, size_t InSize);
};