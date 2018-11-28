#pragma once

using namespace lsgd::thread;
using namespace lsgd::container;

namespace lsgd { namespace async {

	class HBaseGraphTask
	{
	public:
		HBaseGraphTask(uint32 InNumberOfPrerequisities)
			: NumberOfPrerequisitiesOutstanding(InNumberOfPrerequisities)
		{}

		virtual ~HBaseGraphTask() {}

		virtual void Execute() = 0;

		// short-cut for number of prerequisite task left
		HThreadSafeCounter NumberOfPrerequisitiesOutstanding;
	};

	class HGraphEvent
	{
	public:
		HConcurrentQueue<shared_ptr<HBaseGraphTask>> SubsequentList;
		HArray<shared_ptr<HGraphEvent>> EventsToWaitFor;
		HThreadSafeCounter ReferenceCount;
	};

	/*
		comment for 'TaskType'
		
		class TaskType
		{
		public:
			// must-method you should implement!
			void Execute() {}
		};
	*/

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

		virtual void Execute() override;

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

	template <typename TaskType>
	void HGraphTask<TaskType>::Execute()
	{
		check(bTaskCounstructed == true);
		(TaskType*)(&TaskStorage)->Execute();
	}
} }