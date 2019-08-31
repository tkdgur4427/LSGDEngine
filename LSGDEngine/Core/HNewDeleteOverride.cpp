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

void* operator new  (std::size_t count, std::nothrow_t const& tag)
{
	return HGenericMemory::Allocate(count);
}

void* operator new[](std::size_t count, std::nothrow_t const& tag)
{
	return HGenericMemory::Allocate(count);
}

void operator delete  (void* ptr)
{
	if (ptr != nullptr)
		HGenericMemory::Deallocate(ptr);
}

void operator delete[](void* ptr)
{
	if (ptr != nullptr)
		HGenericMemory::Deallocate(ptr);
}

void operator delete  (void* ptr, std::nothrow_t const& tag)
{
	if (ptr != nullptr)
		HGenericMemory::Deallocate(ptr);
}

void operator delete[](void* ptr, std::nothrow_t const& tag)
{
	if (ptr != nullptr)
		HGenericMemory::Deallocate(ptr);
}

void operator delete  (void* ptr, std::size_t sz)
{
	if (ptr != nullptr)
		HGenericMemory::Deallocate(ptr);
}

void operator delete[](void* ptr, std::size_t sz)
{
	if (ptr != nullptr)
		HGenericMemory::Deallocate(ptr);
}