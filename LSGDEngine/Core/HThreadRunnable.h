#pragma once

namespace lsgd { namespace thread {

	class HThreadRunnable
	{
	public:
		virtual void Run() = 0;
	};

} }