#pragma once

typedef signed char        int8;
typedef short              int16;
typedef int                int32;
typedef long long          int64;
typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
typedef unsigned long long uint64;

// @todo - temporary assert functionalties
#define mcheck(condition) if(!(condition)) { *((int*)0) = 3; }

class HMemoryUtil
{
public:
	static uint32 AlignedSize32(uint32 InSize, uint32 InAlignment);
	static bool IsAligned32(uint32 InValue, uint32 InAlignment);
	static void* AlignedPointer(void* InAddress, size_t InAlignment);
	static void* AlignedUpPointer(void* InAddress, size_t InAlignment);
};

// @todo - um...
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <process.h>
#include <stdlib.h>
#include <intrin.h>

#pragma comment(lib, "Ws2_32.lib")

namespace memory { namespace synchronize {

	// @todo - platform independent...
	class HCriticalSection
	{
	public:
		HCriticalSection()
		{
			mcheck(InitializeCriticalSectionAndSpinCount(&CS, 0x00000400));
		}

		~HCriticalSection()
		{
			DeleteCriticalSection(&CS);
		}

		void Lock()
		{
			EnterCriticalSection(&CS);
		}

		void Unlock()
		{
			LeaveCriticalSection(&CS);
		}

		CRITICAL_SECTION CS;
	};

	class HScopedLock
	{
	public:
		HScopedLock(HCriticalSection& InSyncObject)
			: SyncObject(InSyncObject)
		{
			SyncObject.Lock();
		}

		~HScopedLock()
		{
			SyncObject.Unlock();
		}

		HCriticalSection& SyncObject;
	};

} }
