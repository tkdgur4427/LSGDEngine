#include "HCorePCH.h"
#include "HAssert.h"

void Crash()
{
	*((int*)0) = 3;
}

void LogError(const lsgd::HString& message)
{
	lsgd::log::HAsyncLogger::GetSingleton()->LogError(message);
}
