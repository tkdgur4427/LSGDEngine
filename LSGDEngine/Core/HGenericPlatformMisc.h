#pragma once

namespace lsgd {
	
	enum EFileUsageFlag
	{
		Read = (1 << 0),
		Write = (1 << 1),
	};

	class HPlatformFileIO
	{
	public:
		virtual void CreateFile(const HString InFilename, int32 InFileUsage) = 0;
		virtual void WriteFile(void* InBuffer, int64 InSize, int64& OutSize) = 0;
		virtual void ReadFile(void* OutBuffer, int64 InMaxSize, int64& OutSize) = 0;
		virtual void CloseFile() = 0;

		// file usage flag (read | write)
		int32 FileUsageFlag;
	};

	// note that all definition of this class is reside in each platform misc.cpp (e.g. HWindowsPlatformMisc.cpp)
	class HGenericPlatformMisc
	{
	public:
		static unique_ptr<HPlatformFileIO> CreatePlatformFileIO();
	};

}