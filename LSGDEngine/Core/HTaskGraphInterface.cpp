#include "HCorePCH.h"
#include "HTaskGraphInterface.h"

// task thread runnable
#include "HTaskThreadBase.h"

using namespace lsgd;
using namespace lsgd::async;

void HTaskGraph1::Initialize()
{
	// generate task graph threads
	uint32 NumProcssors = HGenericPlatformMisc::GetHardwareThreadNum();
	
	// generate task threads 2x than number of processors
	uint32 TaskThreadNum = NumProcssors * 2; 
	for (uint32 TaskThreadIndex = 0; TaskThreadIndex < TaskThreadNum; ++TaskThreadIndex)
	{
		HTaskThreadBase::TaskThreadSharedContext.CreateTaskThread();
	}
}

void HTaskGraph1::Destroy()
{

}