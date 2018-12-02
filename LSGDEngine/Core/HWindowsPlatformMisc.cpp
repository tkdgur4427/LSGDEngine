#include "HCorePCH.h"
#include "HWIndowsPlatformMisc.h"

#include "HThreadRunnable.h"

using namespace lsgd;

HWindowsPlatformFileIO::HWindowsPlatformFileIO()
	: HPlatformFileIO()
	, FileHandle(nullptr)
{

}

HWindowsPlatformFileIO::~HWindowsPlatformFileIO()
{
	CloseFile();
}

void HWindowsPlatformFileIO::OpenFile(const HString& InFilename, int32 InFileUsage)
{
	Filename = InFilename;
	FileUsageFlag = InFileUsage;

	DWORD UsageFlag = 0;
	UsageFlag |= (FileUsageFlag & EFileUsageFlag::Read) ? GENERIC_READ : 0;
	UsageFlag |= (FileUsageFlag & EFileUsageFlag::Write) ? GENERIC_WRITE : 0;

	DWORD CreateFlag = 0;
	switch (FileUsageFlag)
	{
	case EFileUsageFlag::Read:
		CreateFlag = OPEN_ALWAYS;
		break;
	case EFileUsageFlag::Write:
		CreateFlag = CREATE_ALWAYS;
		break;
	}

	FileHandle = ::CreateFileA(Filename.c_str(), UsageFlag, 0, NULL, CreateFlag, FILE_ATTRIBUTE_NORMAL, NULL);
	
	check(FileHandle != INVALID_HANDLE_VALUE);
}

void HWindowsPlatformFileIO::WriteFile(void* InBuffer, int64 InSize, int64& OutSize)
{
	check(FileHandle != nullptr);
	check((FileUsageFlag & EFileUsageFlag::Write) != 0);

	::WriteFile(FileHandle, InBuffer, InSize, (DWORD*)&OutSize, NULL);
}

void HWindowsPlatformFileIO::ReadFile(void* OutBuffer, int64 InMaxSize, int64& OutSize)
{
	check(FileHandle != nullptr);
	check((FileUsageFlag & EFileUsageFlag::Read) != 0);

	::ReadFile(FileHandle, OutBuffer, InMaxSize, (DWORD*)&OutSize, NULL);
}

void HWindowsPlatformFileIO::CloseFile()
{
	if (FileHandle != nullptr)
	{
		CloseHandle(FileHandle);
	}
}

unique_ptr<HPlatformFileIO> HGenericPlatformMisc::CreatePlatformFileIO()
{
	return make_unique<HPlatformFileIO, HWindowsPlatformFileIO>();
}

unique_ptr<HPlatformThread> HGenericPlatformMisc::CreatePlatformThread(shared_ptr<lsgd::thread::HThreadRunnable>& InThreadRunnable)
{
	return make_unique<HPlatformThread, HWindowsPlatformThread>(InThreadRunnable);
}

HString HGenericPlatformMisc::GetGameDir()
{
	return HString();
}

uint32 HGenericPlatformMisc::GetCurrentThreadId()
{
	HANDLE CurrThreadHandle = nullptr;

	// convert pseudo handle to real handle
	DuplicateHandle(GetCurrentProcess(),
		GetCurrentThread(),
		GetCurrentProcess(),
		&CurrThreadHandle,
		0,
		FALSE,
		DUPLICATE_SAME_ACCESS);

	// return current thread Id
	return GetThreadId(CurrThreadHandle);
}

void HGenericPlatformMisc::Sleep(double InSecs)
{
	_sleep(InSecs * 1000);
}

uint32 HGenericPlatformMisc::GetHardwareThreadNum()
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	return sysInfo.dwNumberOfProcessors;
}

HWindowsPlatformThread::HWindowsPlatformThread(shared_ptr<lsgd::thread::HThreadRunnable>& InRunnable)
	: HPlatformThread(InRunnable)
	, ThreadHandle(0)
{

}

HWindowsPlatformThread::~HWindowsPlatformThread()
{
	Destroy();
}

uint32 __stdcall WindowsThreadEntryPoint(void* InData)
{
	lsgd::thread::HThreadRunnable* Runnable = (lsgd::thread::HThreadRunnable*)InData;
	Runnable->Run();
	_endthreadex(0);
	return 0;
}

bool HWindowsPlatformThread::Create(uint32 CpuCoreAffinity)
{
	// put the option 'SUSPENDED' to set the thread to arbitrary CPU core
	ThreadHandle = (HANDLE)_beginthreadex(nullptr, 0, WindowsThreadEntryPoint, Runnable.get(), CREATE_SUSPENDED, nullptr);
	if (ThreadHandle == 0)
	{
		return false;
	}

	// assign the thread id
	ThreadId = GetThreadId(ThreadHandle);

	// core affinity
	uint32 AffinityMask = 1ull << CpuCoreAffinity;
	SetThreadAffinityMask(ThreadHandle, AffinityMask);

	// now resume the thread
	ResumeThread(ThreadHandle);

	return true;
}

void HWindowsPlatformThread::Destroy()
{
	if (Runnable != nullptr)
	{
		Runnable->Terminate();
	}

	if (ThreadHandle != 0)
	{
		WaitForSingleObject(ThreadHandle, INFINITE);
		CloseHandle(ThreadHandle);
	}	
}

bool HWindowsPlatformThread::IsCurrThread() const
{
	return HGenericPlatformMisc::GetCurrentThreadId() == ThreadId;
}

