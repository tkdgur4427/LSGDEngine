#pragma once

#include "HGenericMemory.h"

#define USE_STD_ALLOCATOR 1
#if USE_STD_ALLOCATOR

// std::allocator version for custom memory management with HGenericMemory
// note that HStdAllocator is template class, so no need to mark as dll export

template <class Type>
class HStdAllocator
{
public:
	typedef size_t size_type;
	typedef int difference_type;
	typedef Type* pointer;
	typedef const Type* const_pointer;
	typedef Type& reference;
	typedef const Type& const_reference;
	typedef Type value_type;

	HStdAllocator() {}
	HStdAllocator(const HStdAllocator&) {}

	pointer allocate(size_type InSize, const void* = 0)
	{
		return (Type*)HGenericMemory::Allocate(InSize);
	}

	void deallocate(void* InPointer, size_type)
	{
		HGenericMemory::Deallocate(InPointer);
	}

	pointer address(reference x) const { return &x; }
	const_pointer address(const_reference x) const { return &x; }

	HStdAllocator<value_type>& operator=(const HStdAllocator&) { return *this; }

	// temporary just disable this not compatible
	//void construct(pointer p, const value_type& val) { new ((pointer)p) value_type(val); }
	//void destroy(pointer p) { p->~value_type(); }

	size_type max_size() const { return size_t(-1); }

	template <class OtherType>
	struct rebind { typedef HStdAllocator<OtherType> other; };
	
	template <class OtherType>
	HStdAllocator(const HStdAllocator<OtherType>&) {}

	template <class OtherType>
	HStdAllocator& operator= (const HStdAllocator<OtherType>&) { return *this; }
};

#endif