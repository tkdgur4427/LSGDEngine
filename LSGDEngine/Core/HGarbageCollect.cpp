#include "HCorePCH.h"
#include "HGarbageCollect.h"
#include "HReferenceCollector.h"
#include "HObjectArray.h"

using namespace lsgd;
using namespace lsgd::gc;

void HGarbageCollect::MarkAndSweep()
{
	// gather the root-set
	HArray<HObject*> RootObjects = HObjectArray::GetSingleton()->GetRootSetObjects();

	// mark
	HReferenceCollector Collector(RootObjects);
	Collector.Collect();

	// sweep

}