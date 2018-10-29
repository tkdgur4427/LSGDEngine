#include "HCorePCH.h"
#include "HTypeOverrides.h"

// primitive type helper
#include "HPrimitiveType.h"

// reflection context
#include "HReflectionContext.h"

using namespace lsgd;
using namespace lsgd::reflect;

HReflectionContext SerializeHelper(HReflectionContext& InContext, void* InPointer, int64 InSize)
{
	InContext.Serialize(InPointer, InSize);
	return InContext;
}
