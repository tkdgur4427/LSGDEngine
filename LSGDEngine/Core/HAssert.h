#pragma once

#include "HLogger.h"

extern void Crash();
extern void LogError(const lsgd::HString& message);

#define check(condition) if(!(condition)) { LogError(#condition); Crash(); }
#define debugf(...) lsgd::log::HAsyncLogger::GetSingleton()->Log(__VA_ARGS__);