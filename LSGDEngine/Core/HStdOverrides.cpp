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

HReflectionContext& operator<<(HReflectionContext& InContext, uint32& Value)
{
	InContext.Serialize(&Value, sizeof(Value));
	return InContext;
}

