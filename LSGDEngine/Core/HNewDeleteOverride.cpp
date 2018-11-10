#include "HCorePCH.h"
#include "HNewDeleteOverride.h"

// generic memory
#include "../Memory/HGenericMemory.h"

void* operator new(std::size_t count)
{
	return HGenericMemory::Allocate(count);
}

void* operator new[](std::size_t count)
{
	return HGenericMemory::Allocate(count);
}

void* operator new  (std::size_t count, const std::nothrow_t& tag)
{
	return HGenericMemory::Allocate(count);
}

void* operator new[](std::size_t count, const std::nothrow_t& tag)
{
	return HGenericMemory::Allocate(count);
}

void operator delete  (void* ptr)
{
	HGenericMemory::Deallocate(ptr);
}

void operator delete[](void* ptr)
{
	HGenericMemory::Deallocate(ptr);
}

void operator delete  (void* ptr, const std::nothrow_t& tag)
{
	HGenericMemory::Deallocate(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t& tag)
{
	HGenericMemory::Deallocate(ptr);
}

void operator delete  (void* ptr, std::size_t sz)
{
	HGenericMemory::Deallocate(ptr);
}

void operator delete[](void* ptr, std::size_t sz)
{
	HGenericMemory::Deallocate(ptr);
}