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

		bool Strcmp(const char* Str1, const char* Str2);
	};
}