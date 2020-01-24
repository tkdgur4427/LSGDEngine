#include "HEnginePCH.h"
#include "HDeltaTimeExecutor.h"

#include "..\Core\HGenericPlatformMisc.h"

using namespace lsgd;

HDeltaTimeExecutor::HDeltaTimeExecutor()
{
	CachedTime = HGenericPlatformMisc::GetSeconds();
}

double HDeltaTimeExecutor::Execute()
{
	double CurrentTime = HGenericPlatformMisc::GetSeconds();
	ElapsedTime = CurrentTime - CachedTime;
	CachedTime = CurrentTime;

	return ElapsedTime;
}