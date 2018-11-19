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
	check(FileUsageFlag & EFileUsageFlag::Read != 0);

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

HString HGenericPlatformMisc::GetGameDir()
{
	return HString();
}

HWindowsPlatformThread::HWindowsPlatformThread(unique_ptr<lsgd::thread::HThreadRunnable>& InRunnable)
	: HPlatformThread(InRunnable)
	, ThreadHandle(0)
{

}

HWindowsPlatformThread::~HWindowsPlatformThread()
{

}

uint32 __stdcall WindowsThreadEntryPoint(void* InData)
{
	lsgd::thread::HThreadRunnable* Runnable = (lsgd::thread::HThreadRunnable*)InData;
	Runnable->Run();
	return 0;
}

bool HWindowsPlatformThread::Create(uint32 CpuCoreAffinity)
{
	// put the option 'SUSPENDED' to set the thread to arbitrary CPU core
	ThreadHandle = _beginthreadex(nullptr, 0, WindowsThreadEntryPoint, Runnable.get(), CREATE_SUSPENDED, nullptr);
	if (ThreadHandle == 0)
	{
		return false;
	}

	return true;
}