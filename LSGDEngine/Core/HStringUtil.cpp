#include "HCorePCH.h"
#include "HStringUtil.h"

using namespace lsgd;

bool HStringUtil::Strcmp(const char* Str1, const char* Str2)
{
	return HStringCompare(Str1, Str2);
}

void HStringUtil::Strcpy(char* Dest, const char* Src)
{
	HStringCopy(Dest, Src);
}

const char* HStringUtil::Strchr(const char* Str, char Char)
{
	return HStrchr(Str, Char);
}

bool HStringUtil::Strncmp(const char* Str1, const char* Str2, int32 NumCharacterToCompare)
{
	return HStringNCompare(Str1, Str2, NumCharacterToCompare);
}
