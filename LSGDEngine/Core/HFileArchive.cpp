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

void HFileCacheChunk::Move(int32 InOffset)
{
	CurrOffset = InOffset;
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
	PlatformFileIO = move(HGenericPlatformMisc::CreatePlatformFileIO());
}

HFileArchive::~HFileArchive()
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
	return CurrFileCacheChunk * HFileCacheChunk::CACHE_SIZE + FileCacheChunks[CurrFileCacheChunk]->Tell();
}

void HFileArchive::Move(int64 Offset)
{
	int32 ChunkIndex = Offset / HFileCacheChunk::CACHE_SIZE;
	int32 ChunkOffset = Offset % HFileCacheChunk::CACHE_SIZE;

	CurrFileCacheChunk = ChunkIndex;
	FileCacheChunks[CurrFileCacheChunk]->Move(ChunkOffset);
}

HFileArchiveWrite::HFileArchiveWrite()
	: HFileArchive()
{
	
}

HFileArchiveWrite::~HFileArchiveWrite()
{

}

void HFileArchiveWrite::UpdateState(int64 Length)
{
	//@todo - need to find more optimized way of using compact size
	if (CurrFileCacheChunk == -1 // when it updates first state
		|| FileCacheChunks[CurrFileCacheChunk]->GetAvailableSize() <= Length)
	{
		// when it is out of memory
		// @todo - trigger new file async flush with this file cache chunk

		// get the new chunk
		CurrFileCacheChunk = GetAvailableFileCacheChunk();
	}
}

void HFileArchiveWrite::Serialize(void* Value, int64 Length)
{
	// update file cache chunk to hold 'Length' size
	UpdateState(Length);
	// write to the file cache chunk
	FileCacheChunks[CurrFileCacheChunk]->Write(Value, Length);
}

HFileArchiveRead::HFileArchiveRead()
	: HFileArchive()
{
	// reading file archive
	bIsSaving = false;
}

HFileArchiveRead::~HFileArchiveRead()
{

}

void HFileArchiveRead::UpdateState(int64 Length)
{
	if (CurrFileCacheChunk == -1 // when it updates first state
		|| FileCacheChunks[CurrFileCacheChunk]->GetAvailableSize() <= Length)
	{
		CurrFileCacheChunk = GetAvailableFileCacheChunk();
	}

	//@todo flush to read the cache size (16K)
}

void HFileArchiveRead::Serialize(void* Value, int64 Length)
{
	UpdateState(Length);
	FileCacheChunks[CurrFileCacheChunk]->Read(Value, Length);
}