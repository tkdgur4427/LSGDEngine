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

		void CreateFile(const HString& InFilename, int32 InFileUsage);
		void WriteFile(void* InBuffer, int64 InSize, int64& OutSize);
		void ReadFile(void* OutBuffer, int64 InMaxSize, int64& OutSize);
		void CloseFile();

		HANDLE FileHandle;
	};

}