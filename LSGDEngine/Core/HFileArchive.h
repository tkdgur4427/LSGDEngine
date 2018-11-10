#pragma once

#include "HReflectionContext.h"

namespace lsgd {
	class HPlatformFileIO;
}

namespace lsgd { namespace fileIO {

	// file cache
	//	- before requesting I/O operation, accumulates the data in memory and dump it all at once
	struct HFileCacheChunk
	{
		HFileCacheChunk()
			: CurrOffset(0)
		{}
		~HFileCacheChunk() {}

		enum
		{
			CACHE_SIZE = 16 * 1024, // 16K
		};

		void Reset();
		uint32 GetAvailableSize() const;
		uint32 Tell() const;
		uint32 Write(void* InData, int32 InSize);
		uint32 Read(void* OutData, int32 InSize);
		void Move(int32 InOffset);

		void* GetData() { return (void*)&Storage[0]; }

		uint8 Storage[CACHE_SIZE];
		uint32 CurrOffset;
	};

	// file IO archive
	class HFileArchive : public lsgd::reflect::HSerializeContext
	{
	public:
		HFileArchive(const HString& InFilename);
		virtual ~HFileArchive();

		int32 GetAvailableFileCacheChunk();
		int64 Tell() const;
		void Move(int64 Offset);

	protected:
		// file cache chunks
		HArray<unique_ptr<HFileCacheChunk>> FileCacheChunks;
		HArray<int32> FreeFileCacheChunks;

		// current read/write file cache chunk
		int32 CurrFileCacheChunk;

		// filename
		HString Filename;

		// platform file io
		unique_ptr<HPlatformFileIO> PlatformFileIO;
	};

	// file IO archive writer
	class HFileArchiveWrite : public HFileArchive
	{
	public:
		HFileArchiveWrite(const HString& InFilename);
		~HFileArchiveWrite();

		void UpdateState(int64 Length);

		virtual void Serialize(void* Value, int64 Length) override;
	};

	// file IO archive reader
	class HFileArchiveRead : public HFileArchive
	{
	public:
		HFileArchiveRead(const HString& InFilename);
		~HFileArchiveRead();

		void UpdateState(int64 Length);
		virtual void Serialize(void* Value, int64 Length) override;
	};
} }