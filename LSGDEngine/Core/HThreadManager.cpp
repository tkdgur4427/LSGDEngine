#include "HCorePCH.h"
#include "HThreadManager.h"

using namespace lsgd;
using namespace lsgd::thread;

void HThreadManager::CreateHardwareThread(shared_ptr<HThreadRunnable> InThreadRunnable)
{
	// create/add hardware thread
	unique_ptr<HPlatformThread> NewHardwareThread = HMove(HGenericPlatformMisc::CreatePlatformThread(InThreadRunnable));
	NewHardwareThread->Create();

	HardwareThreads.push_back(HMove(NewHardwareThread));

	// add software thread
	SoftwareThreads.push_back(InThreadRunnable);
}
