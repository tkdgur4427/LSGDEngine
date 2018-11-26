#pragma once

using namespace lsgd::thread;
using namespace lsgd::container;

namespace lsgd { namespace async {

	class HBaseGraphTask
	{
	public:
		HBaseGraphTask() {}
		virtual ~HBaseGraphTask() {}

		HThreadSafeCounter NumberOfPrerequisitiesOutstanding;
	};

	class HGraphEvent
	{
	public:
		HConcurrentQueue<shared_ptr<HBaseGraphTask>> SubsequentList;
		HArray<shared_ptr<HGraphEvent>> EventsToWaitFor;
		HThreadSafeCounter ReferenceCount;
	};

	template <typename TaskType>
	class HGraphTask : public HBaseGraphTask
	{
	public:
		enum 
		{
			TaskSize = sizeof(TaskType);
		};

		struct HConstructor
		{
			HGraphTask* Owner;
			HArray<shared_ptr<HGraphEvent>> Prerequisites;
		};

		HGraphTask();
		virtual ~HGraphTask();

		shared_ptr<HGraphEvent> Subsequents;
		uint8 TaskStorage[TaskSize];
		bool bTaskCounstructed;
	};

	template <typename TaskType>
	HGraphTask<TaskType>::HGraphTask()
		: HBaseGraphTask()
		, bTaskCounstructed(false)
	{

	}

	template <typename TaskType>
	HGraphTask<TaskType>::~HGraphTask()
	{

	}
} }