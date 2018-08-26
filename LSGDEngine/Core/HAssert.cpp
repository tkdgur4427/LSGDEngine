#include "HCorePCH.h"
#include "HAssert.h"
#include "HLogger.h"

void Crash()
{
	*((int*)0) = 3;
}

void LogError(const lsgd::string& message)
{
	//lsgd::log::HAsyncLogger::GetSingleton()->LogError(message);
}

void Log(const lsgd::string& message)
{
	//lsgd::log::HAsyncLogger::GetSingleton()->Log(message);
}
