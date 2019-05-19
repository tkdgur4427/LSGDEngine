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

		explicit HPlatformThread(shared_ptr<lsgd::thread::HThreadRunnable>& InRunnable)
			: Runnable(InRunnable)
		{
			Runnable->OwnerThread = this;
		}

		virtual ~HPlatformThread(){}

		virtual bool Create(uint32 CpuCoreAffinity = ECpuCoreAffinity::DefaultCoreAffinity) = 0;
		virtual void Destroy() = 0;

		virtual bool IsCurrThread() const = 0;

		// thread slot; ThreadRunnable
		shared_ptr<lsgd::thread::HThreadRunnable> Runnable;
	};

	class HPlatformCriticalSection
	{
	public:
		HPlatformCriticalSection() {}
		virtual ~HPlatformCriticalSection() {}

		virtual void Lock() = 0;
		virtual void Unlock() = 0;
	};

	class HPlatformEvent
	{
	public:
		HPlatformEvent() {}
		virtual ~HPlatformEvent() {}

		virtual void Trigger() = 0;
		virtual void Reset() = 0;
		virtual bool Wait(uint32 WaitTime) = 0;
	};

	// note that all definition of this class is reside in each platform misc.cpp (e.g. HWindowsPlatformMisc.cpp)
	class HGenericPlatformMisc
	{
	public:
		// initialize platform misc
		static void Initialize();

		// @todo - temporary...
		static void GetCPUFrequency();
		static int64 CPUFrequency;

		// get game directory
		static HString GetGameDir();
		// get the shader directory
		static HString GetShaderSourceDir();
		// get the d3d directory
		static HString GetD3DDir();

		// create platform file io instance
		static unique_ptr<HPlatformFileIO> CreatePlatformFileIO();
		// create platform thread instance
		static unique_ptr<HPlatformThread> CreatePlatformThread(shared_ptr<lsgd::thread::HThreadRunnable>& InThreadRunnable);
		// get the current thread Id
		static uint32 GetCurrentThreadId();
		// get the CS
		static unique_ptr< HPlatformCriticalSection> CreateCriticalSection();
		// get the event
		static unique_ptr<HPlatformEvent> CreateEventObject();
		// sleep
		static void Sleep(double InSecs);
		// get the hardware thread
		static uint32 GetHardwareThreadNum();
		// get time
		static double GetSeconds();
	};

	class HGenericPlatformAtomics
	{
	public:
		static int32 HInterlockedCompareSwap(volatile int32* Dest, int32 Exchange, int32 Comparand);
		static int64 HInterlockedCompareSwap(volatile int64* Dest, int64 Exchange, int64 Comparand);
		static int32 HInterlockedAdd(volatile int32* Dest, int32 Value);
		static int64 HInterlockedAdd(volatile int64* Dest, int64 Value);
	};
}