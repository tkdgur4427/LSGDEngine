#pragma once

#include <windows.h>
#include <process.h>
#include <stdlib.h>

#include "HGenericPlatformMisc.h"

namespace lsgd
{

	class HWindowsPlatformFileIO : public HPlatformFileIO
	{
	public:
		HWindowsPlatformFileIO();
		~HWindowsPlatformFileIO();

		virtual void OpenFile(const HString& InFilename, int32 InFileUsage) override;
		virtual void WriteFile(void* InBuffer, int64 InSize, int64& OutSize) override;
		virtual void ReadFile(void* OutBuffer, int64 InMaxSize, int64& OutSize) override;
		virtual void CloseFile();

		HANDLE FileHandle;
	};

	class HWindowsPlatformThread : public HPlatformThread
	{
	public:
		HWindowsPlatformThread(unique_ptr<lsgd::thread::HThreadRunnable>& InRunnable);
		virtual ~HWindowsPlatformThread();

		virtual bool Create(uint32 CpuCoreAffinity) override;
		virtual void Destroy() override;

		// whether the current thread is running or not
		virtual bool IsCurrThread() const override;

		HANDLE ThreadHandle;
		uint32 ThreadId;
	};
}