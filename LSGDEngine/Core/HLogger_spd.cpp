// spd headers
#include "../../ThirdParty/spdlog/spdlog.h"
#include "../../ThirdParty/spdlog/sinks/stdout_color_sinks.h"

using namespace lsgd;
using namespace lsgd::log;

class SpdInstance
{
public:
	lsgd::shared_ptr<spdlog::logger> ConsoleLog;
};

static lsgd::unique_ptr<SpdInstance> gSpdInstance;

template <typename... Arguments>
void HAsyncLogger::Log(const string& InStr, Arguments&&... InArguments)
{
	gSpdInstance->ConsoleLog->info(InStr.c_str(), std::forward<Arguments>(InArguments)...);
}

template <typename... Arguments>
void HAsyncLogger::LogError(const string& InStr, Arguments&&... InArguments)
{
	gSpdInstance->ConsoleLog->info(InStr.c_str(), std::forward<Arguments>(InArguments)...);
	gSpdInstance->ConsoleLog->flush();
}

void HAsyncLogger::CreateLoggerInstance()
{
	gSpdInstance = make_unique<SpdInstance>();
	gSpdInstance->ConsoleLog = spdlog::stderr_color_mt("ConsoleLog");
}
