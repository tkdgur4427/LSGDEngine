#include "HCorePCH.h"
#include "HStdOverrides.h"

// reflection context
#include "HReflectionContext.h"

using namespace lsgd;
using namespace lsgd::reflect;

int32 HStringToStringW(lsgd::HStringW& OutStringW, const lsgd::HString& InString)
{
	HStringW Temp(InString.begin(), InString.end());
	OutStringW = Temp;

	return 0;
}

bool IsLoading(lsgd::reflect::HReflectionContext& InContext)
{
	return InContext.IsLoading();
}

bool IsSaving(lsgd::reflect::HReflectionContext& InContext)
{
	return InContext.IsSaving();
}

HReflectionContext& operator<<(HReflectionContext& InContext, HString& Value)
{
	// serialize the character count
	uint32 Num = static_cast<uint32>(Value.size());
	InContext.Serialize(&Num, sizeof(Num));
	
	uint32 Size = Num * sizeof(char);
	if (InContext.IsLoading())
	{
		// for loading the string, presize the string before copying
		Value.resize(Size);
	}

	// serialize the string content
	InContext.Serialize((void*)Value.data(), Size);

	return InContext;
}

HString HStringPrintf(const char* Format, ...)
{
	const uint32 BufSize = 1024; // @todo - temporary
	char Buffer[BufSize];

	va_list ArgList;
	va_start(ArgList, Format);
	std::vsnprintf(Buffer, BufSize, Format, ArgList);
	va_end(ArgList);

	HString Result;
	Result.append(Buffer);

	return Result;
}

bool HStringCompare(const char* Src0, const char* Src1)
{
	return strcmp(Src0, Src1) == 0;
}

void HStringCopy(char* Dest, const char* Src)
{
	strcpy(Dest, Src);
}

const char* HStrchr(const char* Str, int32 Char)
{
	return strchr(Str, Char);
}

bool HStringNCompare(const char* Str1, const char* Str2, int32 NumCharacterToCompare)
{
	return strncmp(Str1, Str2, NumCharacterToCompare) == 0;
}