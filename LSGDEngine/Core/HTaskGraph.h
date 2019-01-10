#pragma once

using namespace lsgd::thread;
using namespace lsgd::container;

namespace lsgd { namespace async {

	class HGraphEvent;

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
		virtual void Unlock() = 0;
		virtual shared_ptr<HGraphEvent> GetCompletionEvent() = 0;

		// methods
		void SetupCompletePrerequisities(bool bUnlock = true);
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
			// after successfully create task dispatch right away
			template <typename... Args>
			shared_ptr<HGraphEvent> ConstructAndDispatchWhenReady(Args&&... InArgs)
			{
				HGraphTask* RawOwner = (HGraphTask*)(Owner.get());

				// construct the task instance into task storage
				new ((void*)(&(RawOwner->TaskStorage[0]))) TaskType(std::forward<Args>(InArgs)...);
				// set up the HGraphTask
				return RawOwner->Setup(Prerequisites);
			}

			// after create task, just hold it
			template <typename... Args>
			shared_ptr<HBaseGraphTask> ConstructAndHold(Args&&... InArgs)
			{
				HGraphTask* RawOwner = (HGraphTask*)(Owner.get());

				// construct the task instance into task storage
				new ((void*)(&(RawOwner->TaskStorage[0]))) TaskType(std::forward<Args>(InArgs)...);
				// set up the HGraphTask (note that 
				RawOwner->Setup(Prerequisites, false);

				return Owner;
			}

			HConstructor(shared_ptr<HBaseGraphTask> InOwner, HArray<shared_ptr<HGraphEvent>>& InPrerequisitiesRef)
				: Owner(InOwner)
				, Prerequisites(InPrerequisitiesRef)
			{}

			shared_ptr<HBaseGraphTask> Owner;
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
			
			return HConstructor(NewTask, InPrerequisites);
		}

		virtual ~HGraphTask();
		virtual void Execute() override;
		virtual void Unlock() override;

		// get subsequent completion handle
		virtual shared_ptr<HGraphEvent> GetCompletionEvent() { return Subsequents; }

		// helper methods
		shared_ptr<HGraphEvent> Setup(HArray<shared_ptr<HGraphEvent>>& InPrerequisites, bool bUnlock = true);
		void SetupPrerequisites(HArray<shared_ptr<HGraphEvent>>& InPrerequisites, bool bUnlock = true);

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
	void HGraphTask<TaskType>::Unlock()
	{
		ConditionalQueueTask();
	}

	template <typename TaskType>
	void HGraphTask<TaskType>::SetupPrerequisites(HArray<shared_ptr<HGraphEvent>>& InPrerequisites, bool bUnlock)
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

		// if bUnlock is false, skip the queue the task to the task queue
		SetupCompletePrerequisities(bUnlock);
	}

	template <typename TaskType>
	shared_ptr<HGraphEvent> HGraphTask<TaskType>::Setup(HArray<shared_ptr<HGraphEvent>>& InPrerequisites, bool bUnlock)
	{
		shared_ptr<HGraphEvent> ReturnGraphEvent = Subsequents;
		SetupPrerequisites(InPrerequisites, bUnlock);
		return ReturnGraphEvent;
	}
} }