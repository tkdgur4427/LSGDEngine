#include "HCorePCH.h"
#include "HWIndowsPlatformMisc.h"

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

void HWindowsPlatformFileIO::CreateFile(const HString InFilename, int32 InFileUsage)
{
	FileUsageFlag = InFileUsage;

	DWORD UsageFlag = 0;
	UsageFlag |= (FileUsageFlag & EFileUsageFlag::Read) ? GENERIC_READ : 0;
	UsageFlag |= (FileUsageFlag & EFileUsageFlag::Write) ? GENERIC_WRITE : 0;

	FileHandle = ::CreateFileA(InFilename.c_str(), UsageFlag, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	check(FileHandle != INVALID_HANDLE_VALUE);
}

void HWindowsPlatformFileIO::WriteFile(void* InBuffer, int64 InSize, int64& OutSize)
{
	check(FileHandle != nullptr);
	check(FileUsageFlag & EFileUsageFlag::Write != 0);

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