#pragma once

namespace lsgd { namespace log {
	class HAsyncLogger
	{
	public:
		static HAsyncLogger* GetSingleton()
		{
			static HAsyncLogger Logger;
			return &Logger;
		}

		struct LoggerInstance {};

		template <typename... Arguments>
		void Log(const string& InStr, Arguments&&... InArguments);

		template <typename... Arguments>
		void LogError(const string& InStr, Arguments&&... InArguments);

	protected:
		void CreateLoggerInstance();

		HAsyncLogger() 
		{
			CreateLoggerInstance();
		}

		unique_ptr<LoggerInstance> Instance;
	};
}
}

#include "HLogger_spd.cpp"
