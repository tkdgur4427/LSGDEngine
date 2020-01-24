#pragma once

namespace lsgd
{
	class HDeltaTimeExecutor
	{
	public:
		HDeltaTimeExecutor();

		double Execute();

	protected:
		double ElapsedTime;
		double CachedTime;
	};
}

