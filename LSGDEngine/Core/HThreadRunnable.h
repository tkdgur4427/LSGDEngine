#pragma once

// forward declaration
namespace lsgd
{
	class HPlatformThread;
}

namespace lsgd { namespace thread {

	/*
		you can think of 'HThreadRunnable' as software thread
	*/

	class HThreadRunnable : public enable_shared_from_this<HThreadRunnable>
	{
	public:
		HThreadRunnable(const HString& InName)
			: Name(InName)
		{}
		virtual ~HThreadRunnable() {}

		virtual void Run() = 0;
		virtual void Terminate() = 0;

		// whether the thread runnable is running binded hardware thread
		bool IsCurrThread() const;

		// currently binded thread
		HPlatformThread* OwnerThread;

		// thread name
		HString Name;
	};
} }