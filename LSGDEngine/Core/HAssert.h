#pragma once

extern void Crash();
extern void LogError(const lsgd::string& message);
extern void Log(const lsgd::string& message);

#define check(condition) if(!(condition)) { LogError(#condition); Crash(); }
#define debug(message) Log(#message);