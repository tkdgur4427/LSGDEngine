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
		void Log(const HString& InStr, Arguments&&... InArguments)
		{
			HScopedLock Lock(CS);
			printf(InStr.c_str(), InArguments...);
		}

		template <typename... Arguments>
		void LogError(const HString& InStr, Arguments&&... InArguments)
		{
			HScopedLock Lock(CS);
			printf(InStr.c_str(), InArguments...);
		}

	protected:
		HAsyncLogger() 
		{
			
		}

		HCriticalSection CS;
	};
}
}
