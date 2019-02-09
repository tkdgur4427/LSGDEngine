#include "HCorePCH.h"
#include "HStdOverrides.h"

// reflection context
#include "HReflectionContext.h"

using namespace lsgd;
using namespace lsgd::reflect;

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