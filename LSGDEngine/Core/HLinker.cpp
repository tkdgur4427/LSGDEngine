#include "HCorePCH.h"
#include "HLinker.h"

using namespace lsgd;
using namespace lsgd::reflect;

HReflectionContext& operator<<(HReflectionContext& InContext, HObjectExport& InExport)
{
	InContext << InExport.OuterIndex;
	InContext << InExport.ObjectName;
	InContext << InExport.SerialOffset;
	InContext << InExport.SerialSize;

	return InContext;
}

HReflectionContext& operator<<(HReflectionContext& InContext, HObjectImport& InImport)
{
	InContext << InImport.OuterIndex;
	InContext << InImport.SourceIndex;
	InContext << InImport.ObjectName;

	return InContext;
}