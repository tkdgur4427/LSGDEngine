#include <stdlib.h>

namespace lsgd
{
	class HCString
	{
	public:
		static void Strcpy(char* Dest, const char* const Src)
		{
			strcpy(Dest, Src);
		}

		static uint32 Strlen(const char* InStr)
		{
			return strlen(InStr);
		}

		static void Itoa(int32 InNumber, char* OutCString, int32 InBase = 10)
		{
			itoa(InNumber, OutCString, InBase);
		}

		static int32 Strcmp(const char* const Str0, const char* const Str1)
		{
			return strcmp(Str0, Str1);
		}
	};
}