#include "HCorePCH.h"
#include "HFileArchive.h"

using namespace lsgd;
using namespace lsgd::fileIO;

void HFileCacheChunk::Reset()
{
	// reset just by setting 0 as CurrOffset
	CurrOffset = 0;
}

uint32 HFileCacheChunk::GetAvailableSize() const
{
	return (uint32)CACHE_SIZE - Tell();
}

uint32 HFileCacheChunk::Tell() const
{
	return CurrOffset;
}

uint32 HFileCacheChunk::Write(void* InData, int32 InSize)
{
	if (GetAvailableSize() < InSize)
	{
		// < 0, indicate failed to write the data
		return -1;
	}

	// copy the data to the cache 
	HGenericMemory::MemCopy((void*)&Storage[0], InData, InSize);

	// update the offset
	CurrOffset += InSize;

	return CurrOffset;
}

uint32 HFileCacheChunk::Read(void* OutData, int32 InSize)
{
	if (Tell() >= CACHE_SIZE)
	{
		return -1;
	}

	// copy the data to the OutData
	HGenericMemory::MemCopy(OutData, (void*)&Storage[0], InSize);

	// update the read offset
	CurrOffset += InSize;

	return CurrOffset;
}

HFileArchive::HFileArchive()
	: CurrFileCacheChunk(-1)
{

}

int32 HFileArchive::GetAvailableFileCacheChunk()
{
	if (FreeFileCacheChunks.size() > 0)
	{
		int32 Result = FreeFileCacheChunks.back();
		FreeFileCacheChunks.pop_back();
		return Result;
	}

	unique_ptr<HFileCacheChunk> NewChunk = make_unique<HFileCacheChunk>();
	
	int32 Result = FileCacheChunks.size();
	FileCacheChunks.push_back(move(NewChunk));

	return Result;
}

int64 HFileArchive::Tell() const
{
	if (CurrFileCacheChunk != -1)
	{
		int64 CurrChunkSize = CurrFileCacheChunk * HFileCacheChunk::CACHE_SIZE;
		return CurrChunkSize + FileCacheChunks[CurrFileCacheChunk]->Tell();
	}

	// invalid tell
	return 0;
}

void HFileArchive::UpdateWriteState(int64 Length)
{
	// if there is first initialization, get the new chunk
	if (CurrFileCacheChunk == -1)
	{
		CurrFileCacheChunk = GetAvailableFileCacheChunk();
		return;
	}

	//@todo - need to find more optimized way of using compact size
	if (FileCacheChunks[CurrFileCacheChunk]->GetAvailableSize() <= Length)
	{
		// when it is out of memory
		// @todo - trigger new file async flush with this file cache chunk

		// get the new chunk
		CurrFileCacheChunk = GetAvailableFileCacheChunk();
		return;
	}
}

void HFileArchive::Serialize(void* Value, int64 Length)
{
	if (IsSaving())
	{
		// update file cache chunk to hold 'Length' size
		UpdateWriteState(Length);
		// write to the file cache chunk
		FileCacheChunks[CurrFileCacheChunk]->Write(Value, Length);
	}
	else
	{
		// todo! need to also implement correct precaching from file
	}
}