#pragma once

namespace lsgd { namespace thread {

	/*
		you can think of 'HThreadRunnable' as software thread
	*/

	class HThreadRunnable
	{
	public:
		HThreadRunnable() {}
		virtual ~HThreadRunnable() {}

		virtual void Run() = 0;
		virtual void Terminate() = 0;
	};
} }