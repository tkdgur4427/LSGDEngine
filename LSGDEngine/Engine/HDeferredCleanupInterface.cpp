#include "HEnginePCH.h"
#include "HDeferredCleanupInterface.h"

using namespace lsgd;
using namespace lsgd::container;

// the set of deferred cleanup objects which are pending cleanup
static HConcurrentQueue<HDeferredCleanupInterface*> PendingCleanupObjectsList;

HPendingCleanupObjects::HPendingCleanupObjects()
{
	PendingCleanupObjectsList.PopAll(CleanupArray);
}

HPendingCleanupObjects::~HPendingCleanupObjects()
{
	for (auto& PendingCleanupObject : CleanupArray)
	{
		PendingCleanupObject->FinishCleanup();
	}	
}

void BeginCleanup(HDeferredCleanupInterface* CleanupObject)
{
	PendingCleanupObjectsList.Push(CleanupObject);
}
