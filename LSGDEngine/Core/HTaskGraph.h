#pragma once

using namespace lsgd::thread;
using namespace lsgd::container;

namespace lsgd { namespace async {

	/*
		note that
			- for preventing dangling pointer situation, I add enable_shared_from_this class (different from UE4; compared to raw pointer management)
	*/
	class HBaseGraphTask : public enable_shared_from_this<HBaseGraphTask>
	{
	public:
		HBaseGraphTask(uint32 InNumberOfPrerequisities)
			: NumberOfPrerequisitiesOutstanding(InNumberOfPrerequisities)
		{}

		virtual ~HBaseGraphTask() {}

		virtual void Execute() = 0;

		// methods
		void SetupCompletePrerequisities();
		void ConditionalQueueTask();
		void QueueTask();
		void QueueTaskToNamedThread();

		// short-cut for number of prerequisite task left
		HThreadSafeCounter NumberOfPrerequisitiesOutstanding;
				
		// queuing task to different queue
		bool bNamedThread;
		HString NamedThreadName;
	};

	class HGraphEvent
	{
	public:
		HGraphEvent();
		~HGraphEvent();

		static shared_ptr<HGraphEvent> CreateGraphEvent();

		// methods
		bool AddSubsequent(shared_ptr<HBaseGraphTask> InSubsequent);

		HClosableConcurrentQueue<shared_ptr<HBaseGraphTask>> SubsequentList;
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
			TaskSize = sizeof(TaskType),
		};

		struct HConstructor
		{
			template <typename... Args>
			shared_ptr<HGraphEvent> ConstructAndDispatchWhenReady(Args&&... InArgs)
			{
				// construct the task instance into task storage
				new ((void*)Owner->TaskStorage[0]) TaskType(std::forward<Args>(InArgs)...);
				// set up the HGraphTask
				return Owner->Setup(Prerequisites);				
			}

			HConstructor(HGraphTask* InOwner, HArray<shared_ptr<HGraphEvent>>& InPrerequisitiesRef)
				: Owner(InOwner)
				, Prerequisites(InPrerequisitiesRef)
			{}

			HGraphTask* Owner;
			HArray<shared_ptr<HGraphEvent>>& Prerequisites;
		};

		// static method
		static HConstructor CreateTask(HArray<shared_ptr<HGraphEvent>>& InPrerequisites, bool InbNamedThread = false, const HString& InNamedThreadName = "")
		{
			uint32 NumberOfPrerequisites = InPrerequisites.size();
			shared_ptr<HBaseGraphTask> NewTask = make_shared<HBaseGraphTask, HGraphTask>(HGraphEvent::CreateGraphEvent(), NumberOfPrerequisites);
			HGraphTask* NewGraphTask = (HGraphTask*)(NewTask.get());

			NewGraphTask->bNamedThread = InbNamedThread;
			NewGraphTask->NamedThreadName = InNamedThreadName;
			
			return HConstructor(NewGraphTask, InPrerequisites);
		}

		virtual ~HGraphTask();
		virtual void Execute() override;

		// helper methods
		shared_ptr<HGraphEvent> Setup(HArray<shared_ptr<HGraphEvent>>& InPrerequisites);
		void SetupPrerequisites(HArray<shared_ptr<HGraphEvent>>& InPrerequisites);

		shared_ptr<HGraphEvent> Subsequents;
		uint8 TaskStorage[TaskSize];
		bool bTaskCounstructed;

		// make it only allow to create by CreateTask static method
		HGraphTask(shared_ptr<HGraphEvent>& InSubsequents, uint32 InNumberOfPrerequisities);
	};

	template <typename TaskType>
	HGraphTask<TaskType>::HGraphTask(shared_ptr<HGraphEvent>& InSubsequents, uint32 InNumberOfPrerequisities)
		: HBaseGraphTask(InNumberOfPrerequisities)
		, bTaskCounstructed(false)
	{
		Subsequents.swap(InSubsequents);
		check(InSubsequents.get() == nullptr);
	}

	template <typename TaskType>
	HGraphTask<TaskType>::~HGraphTask()
	{

	}

	template <typename TaskType>
	void HGraphTask<TaskType>::Execute()
	{
		check(bTaskCounstructed == true);
		((TaskType*)(&TaskStorage))->Execute();
	}

	template <typename TaskType>
	void HGraphTask<TaskType>::SetupPrerequisites(HArray<shared_ptr<HGraphEvent>>& InPrerequisites)
	{
		// now set up task is constructed
		bTaskCounstructed = true;

		int32 AlreadyCompletedPrerequisites = 0;
		for (auto&& Prerequisite : InPrerequisites)
		{
			check(Prerequisite != nullptr);
			if (!Prerequisite->AddSubsequent(shared_from_this()))
			{
				AlreadyCompletedPrerequisites++;
			}
		}
		check((InPrerequisites.size() - AlreadyCompletedPrerequisites) == NumberOfPrerequisitiesOutstanding.GetValue());

		SetupCompletePrerequisities();
	}

	template <typename TaskType>
	shared_ptr<HGraphEvent> HGraphTask<TaskType>::Setup(HArray<shared_ptr<HGraphEvent>>& InPrerequisites)
	{
		shared_ptr<HGraphEvent> ReturnGraphEvent = Subsequents;
		SetupPrerequisites(InPrerequisites);
		return ReturnGraphEvent;
	}
} }