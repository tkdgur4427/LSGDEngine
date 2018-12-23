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

uint32 HFileCacheChunk::Write(void* InData, int64 InSize)
{
	if (GetAvailableSize() < InSize)
	{
		// < 0, indicate failed to write the data
		return -1;
	}

	// copy the data to the cache 
	HGenericMemory::MemCopy((void*)&Storage[CurrOffset], InData, (size_t)InSize);

	// update the offset
	CurrOffset += (uint32)InSize;

	return CurrOffset;
}

void HFileCacheChunk::Move(int32 InOffset)
{
	CurrOffset = InOffset;
}

uint32 HFileCacheChunk::Read(void* OutData, int64 InSize)
{
	if (Tell() >= CACHE_SIZE)
	{
		return -1;
	}

	// copy the data to the OutData
	HGenericMemory::MemCopy(OutData, (void*)&Storage[CurrOffset], (size_t)InSize);

	// update the read offset
	CurrOffset += (uint32)InSize;

	return CurrOffset;
}

HFileArchive::HFileArchive(const HString& InFilename)
	: CurrFileCacheChunk(-1)
	, Filename(InFilename)
{
	PlatformFileIO = HMove(HGenericPlatformMisc::CreatePlatformFileIO());

	// get the new chunk (initially allocate new file cache chunk)
	CurrFileCacheChunk = GetAvailableFileCacheChunk();
}

HFileArchive::~HFileArchive()
{
	PlatformFileIO->CloseFile();
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
	FileCacheChunks.push_back(HMove(NewChunk));

	return Result;
}

int64 HFileArchive::Tell() const
{
	if (CurrFileCacheChunk == -1)
	{
		return 0;
	}

	return CurrFileCacheChunk * HFileCacheChunk::CACHE_SIZE + FileCacheChunks[CurrFileCacheChunk]->Tell();
}

void HFileArchive::Move(int64 Offset)
{
	int32 ChunkIndex = (int32)Offset / HFileCacheChunk::CACHE_SIZE;
	int32 ChunkOffset = Offset % HFileCacheChunk::CACHE_SIZE;

	CurrFileCacheChunk = ChunkIndex;
	FileCacheChunks[CurrFileCacheChunk]->Move(ChunkOffset);
}

HFileArchiveWrite::HFileArchiveWrite(const HString& InFilename)
	: HFileArchive(InFilename)
{
	// writing file archive
	bIsSaving = true;

	PlatformFileIO->OpenFile(InFilename, EFileUsageFlag::Write);
}

HFileArchiveWrite::~HFileArchiveWrite()
{
	// @todo - need to refactor this code
	// flush rest data to the file
	int64 WrittenSize;
	PlatformFileIO->WriteFile(FileCacheChunks[CurrFileCacheChunk]->GetData(), FileCacheChunks[CurrFileCacheChunk]->Tell(), WrittenSize);
}

void HFileArchiveWrite::UpdateState(int64 Length)
{
	//@todo - need to find more optimized way of using compact size
	if (CurrFileCacheChunk == -1 // when it updates first state
		|| FileCacheChunks[CurrFileCacheChunk]->GetAvailableSize() <= Length)
	{
		// when it is out of memory, write to the file
		//	- @todo need to abstract this logic as flush
		int64 WrittenSize;
		PlatformFileIO->WriteFile(FileCacheChunks[CurrFileCacheChunk]->GetData(), HFileCacheChunk::CACHE_SIZE, WrittenSize);

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

HFileArchiveRead::HFileArchiveRead(const HString& InFilename)
	: HFileArchive(InFilename)
{
	// reading file archive
	bIsSaving = false;

	// create file handle for read
	PlatformFileIO->OpenFile(InFilename, EFileUsageFlag::Read);
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

	// read the file IO from the specified file
	int64 ReadSize = 0;
	PlatformFileIO->ReadFile(FileCacheChunks[CurrFileCacheChunk]->GetData(), HFileCacheChunk::CACHE_SIZE, ReadSize);
}

void HFileArchiveRead::Serialize(void* Value, int64 Length)
{
	UpdateState(Length);
	FileCacheChunks[CurrFileCacheChunk]->Read(Value, Length);
}