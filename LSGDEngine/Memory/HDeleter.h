#pragma once

#include "HGenericMemory.h"

template <typename Type>
class HStdDeleter
{
public:
	void operator() (Type* InPtr) const
	{
		HGenericMemory::Deallocate(InPtr);
	}
};