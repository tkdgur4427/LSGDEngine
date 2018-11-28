#include "HCorePCH.h"
#include "HThreadRunnable.h"

using namespace lsgd::thread;

bool HThreadRunnable::IsCurrThread() const
{
	check(OwnerThread != nullptr);
	return (OwnerThread->IsCurrThread());
}
