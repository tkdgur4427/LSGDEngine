#pragma once

#include "HThreadRunnable.h"

namespace lsgd { namespace thread {

	// thread manager class for managing hardware/software threads
	class HThreadManager
	{
	public:
		static HThreadManager* GetSingleton()
		{
			static HThreadManager ThreadManagerInstance;
			return &ThreadManagerInstance;
		}

		void CreateHardwareThread(shared_ptr<HThreadRunnable> InThreadRunnable);
	
		//@todo - need to tick function to check software thread's ref count == 1, need to pop out and delete hardware thread

	protected:
		HThreadManager() {}
		~HThreadManager() {}

		/*
			currently, the number of hardware threads and software threads are same!
				- but, in the future, need to make differentiations; it means that I will instroduce fiber concepts
		*/

		// hardware threads
		HArray<unique_ptr<HPlatformThread>>  HardwareThreads;

		// software threads
		//	- note that real instance life time is managed by HPlatformThread
		HArray<shared_ptr<HThreadRunnable>> SoftwareThreads;
	};

} }
