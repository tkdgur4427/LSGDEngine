#include <stdlib.h>

namespace lsgd
{
	class HCString
	{
	public:
		static void Strcpy(char* Dest, uint32 DestSize, const char* const Src)
		{
			strcpy_s(Dest, DestSize, Src);
		}

		static uint32 Strlen(const char* InStr)
		{
			return strlen(InStr);
		}

		static void Itoa(int32 InNumber, char* OutCString, int32 Size, int32 InBase = 10)
		{
			_itoa_s(InNumber, OutCString, Size, InBase);
		}

		static void Atoi(char* InCString, int32& OutNumber)
		{
			OutNumber = atoi(InCString);
		}

		static int32 Strcmp(const char* const Str0, const char* const Str1)
		{
			return strcmp(Str0, Str1);
		}
	};
}