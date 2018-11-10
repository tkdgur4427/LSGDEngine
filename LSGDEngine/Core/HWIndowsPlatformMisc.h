#pragma once

#include <windows.h>
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

}