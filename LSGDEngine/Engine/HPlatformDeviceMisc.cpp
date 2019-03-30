#include "HEnginePCH.h"
#include "HPlatformDeviceMisc.h"

#include "HWindowFrameWindows.h"

using namespace lsgd;

unique_ptr<HWindowFrame> HPlatformDeviceMisc::CreateWindowFrame()
{
	return make_unique<HWindowFrame, HWindowFrameWindows>();
}