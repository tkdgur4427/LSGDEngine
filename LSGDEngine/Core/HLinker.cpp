#include "HCorePCH.h"
#include "HLinker.h"

#include "HReflectionContext.h"

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

HReflectionContext& operator<<(HReflectionContext& InContext, HPackageTOC& InTOC)
{
	InContext << InTOC.TotalHeaderSize;
	InContext << InTOC.ExportOffset;
	InContext << InTOC.ExportCount;
	InContext << InTOC.ImportOffset;
	InContext << InTOC.ImportCount;
	InContext << InTOC.NameOffset;
	InContext << InTOC.NameCount;
	InContext << InTOC.DependsOffset;

	return InContext;
}