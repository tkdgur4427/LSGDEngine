// spd headers
//#include "../../ThirdParty/spdlog/spdlog.h"
//#include "../../ThirdParty/spdlog/sinks/stdout_color_sinks.h"

using namespace lsgd;
using namespace lsgd::log;

class SpdInstance : public HAsyncLogger::LoggerInstance
{
public:
	//lsgd::shared_ptr<spdlog::logger> ConsoleLog;
};

template <typename... Arguments>
void HAsyncLogger::Log(const HString& InStr, Arguments&&... InArguments)
{
	//SpdInstance* WeakInstance = (SpdInstance*)Instance.get();
	//WeakInstance->ConsoleLog->info(InStr.c_str(), std::forward<Arguments>(InArguments)...);
}

template <typename... Arguments>
void HAsyncLogger::LogError(const HString& InStr, Arguments&&... InArguments)
{
	//SpdInstance* WeakInstance = (SpdInstance*)Instance.get();
	//WeakInstance->ConsoleLog->info(InStr.c_str(), std::forward<Arguments>(InArguments)...);
	//WeakInstance->ConsoleLog->flush();
}

void HAsyncLogger::CreateLoggerInstance()
{
	Instance = make_unique<LoggerInstance, SpdInstance>();

	SpdInstance* WeakInstance = (SpdInstance*)Instance.get();
	//WeakInstance->ConsoleLog = spdlog::stderr_color_mt("ConsoleLog");
}
