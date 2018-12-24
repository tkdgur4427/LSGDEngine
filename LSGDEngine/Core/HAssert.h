#pragma once

extern void Crash();
extern void LogError(const lsgd::HString& message);
extern void Log(const lsgd::HString& message);

#define check(condition) if(!(condition)) { LogError(#condition); Crash(); }
#define debugf(message) Log(#message);