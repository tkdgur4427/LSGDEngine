#include "HCorePCH.h"
#include "HStdOverrides.h"

// reflection context
#include "HReflectionContext.h"

using namespace lsgd;
using namespace lsgd::reflect;

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
}