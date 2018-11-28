#pragma once

#include "HThreadRunnable.h"

namespace lsgd {
	
	enum EFileUsageFlag
	{
		Read = (1 << 0),
		Write = (1 << 1),
	};

	class HPlatformFileIO
	{
	public:
		virtual void OpenFile(const HString& InFilename, int32 InFileUsage) = 0;
		virtual void WriteFile(void* InBuffer, int64 InSize, int64& OutSize) = 0;
		virtual void ReadFile(void* OutBuffer, int64 InMaxSize, int64& OutSize) = 0;
		virtual void CloseFile() = 0;

		// filename
		HString Filename;

		// file usage flag (read | write)
		int32 FileUsageFlag;
	};

	class HPlatformThread
	{
	public:
		enum ECpuCoreAffinity
		{
			Core0 = (1ull << 0),
			Core1 = (1ull << 1),
			Core2 = (1ull << 2),
			Core3 = (1ull << 3),
			Core4 = (1ull << 4),
			Core5 = (1ull << 5),
			Core6 = (1ull << 6),
			Core7 = (1ull << 7),
			DefaultCoreAffinity = (Core0 | Core1 | Core2 | Core3 | Core4 | Core5 | Core6 | Core7),
		};

		explicit HPlatformThread(unique_ptr<lsgd::thread::HThreadRunnable>& InRunnable)
			: Runnable(move(InRunnable))
		{}

		virtual ~HPlatformThread(){}

		virtual bool Create(uint32 CpuCoreAffinity = ECpuCoreAffinity::DefaultCoreAffinity) = 0;
		virtual void Destroy() = 0;

		virtual bool IsCurrThread() const = 0;

		// thread slot; ThreadRunnable
		shared_ptr<lsgd::thread::HThreadRunnable> Runnable;
	};

	// note that all definition of this class is reside in each platform misc.cpp (e.g. HWindowsPlatformMisc.cpp)
	class HGenericPlatformMisc
	{
	public:
		// get game directory
		static HString GetGameDir();
		// create platform file io instance
		static unique_ptr<HPlatformFileIO> CreatePlatformFileIO();
		// create platform thread instance
		static unique_ptr<HPlatformThread> CreatePlatformThread(unique_ptr<lsgd::thread::HThreadRunnable>& InThreadRunnable);
		// get the current thread Id
		static uint32 GetCurrentThreadId();
	};

}