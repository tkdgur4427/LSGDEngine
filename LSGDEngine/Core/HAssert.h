#pragma once

#include "HLogger.h"

inline void Crash()
{
	*((int*)0) = 3;
}

#define check(condition) if(!condition) { lsgd::log::HAsyncLogger::GetSingleton()->LogError(#condition); Crash(); }

#define debug(message) lsgd::log::HAsyncLogger::GetSingleton()->Log(#message);