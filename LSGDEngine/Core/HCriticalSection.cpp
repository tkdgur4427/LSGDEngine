#include "HCorePCH.h"
#include "HCriticalSection.h"

using namespace lsgd;

HCriticalSection::HCriticalSection()
{
	CS = HMove(HGenericPlatformMisc::CreateCriticalSection());
}

HCriticalSection::~HCriticalSection()
{
	
}

void HCriticalSection::Lock()
{
	CS->Lock();
}

void HCriticalSection::Unlock()
{
	CS->Unlock();
}