#pragma once

namespace lsgd
{
	// utility class for HString helper methods
	class HStringUtil
	{
	public:
		template <typename... ArgType>
		static HString Printf(const char* Format, ArgType&&... Args)
		{
			return HStringPrintf(Format, Args...);
		}

		static bool Strcmp(const char* Str1, const char* Str2);
		static void Strcpy(char* Dest, const char* Src);
		static const char* Strchr(const char* Str, char Chars);
		static bool Strncmp(const char* Str1, const char* Str2, int32 NumCharacterToCompare);
	};
}