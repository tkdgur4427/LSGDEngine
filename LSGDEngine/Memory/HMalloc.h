#pragma once

class HMalloc
{
public:
	virtual void* Malloc(size_t Count, uint32 Alignment = 16) = 0;
	virtual void* Realloc(void* Original, size_t Count, uint32 Alignment = 4) = 0;
	virtual bool Free(void* Original) = 0;
};