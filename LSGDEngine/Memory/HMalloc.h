#pragma once

class HMalloc
{
public:
	virtual void* Malloc(size_t Count, unsigned int Alignment = 4) = 0;
	virtual void* Realloc(void* Original, size_t Count, unsigned int Alignment = 4) = 0;
	virtual void Free(void* Original) = 0;
};