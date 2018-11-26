#pragma once

#include "HThreadRunnable.h"
#include "HTaskGraph.h"

using namespace lsgd::thread;
using namespace lsgd::container;

namespace lsgd { namespace async {

	/*
		different from UE4, I separate this context as another layer abstraction
			- we could maybe separate this another big class, for now, I just leave it as static member variable in HTaskThreadBase
	*/
	class HTaskThreadSharedContext
	{
	public:
		uint32 GenerateTaskThreadId();

		HConcurrentQueue<shared_ptr<HBaseGraphTask>> Tasks;

		// task thread base id generator
		HThreadSafeCounter TaskThreadIdGenerator;
	};

	class HTaskThreadBase : public HThreadRunnable
	{
	public:
		// shared task-thread context
		static HTaskThreadSharedContext TaskThreadSharedContext;

		// member variables

		// generated from HTaskThreadSharedContext::
		uint32 UniqueId;
	};

} }