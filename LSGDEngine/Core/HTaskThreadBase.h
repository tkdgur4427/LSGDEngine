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
		
		uint32 AllocTaskThreadLS();

		shared_ptr<HThreadRunnable> GetTaskThread();

		shared_ptr<HBaseGraphTask> GetNextTask();

		shared_ptr<HThreadRunnable> GetNamedThread();

		shared_ptr<HThreadRunnable> GetNamedThread(const HString& InName);

		// enqueue/dequeue the graph task to the queue
		void EnqueueGraphTask(shared_ptr<HBaseGraphTask> InTask);
		shared_ptr<HBaseGraphTask> DequeueGraphTask();

		// enqueue the graph task from named thread's own queue
		void EnqueueGraphTaskToNamedThread(const HString& InNamedThread, shared_ptr<HBaseGraphTask> InTask);

		// generate task threads
		void CreateTaskThread();

		// generate named threads
		void CreateNamedThread(const HString& InName);

	protected:
		int32 GetTaskThreadIndex();
		int32 GetNamedThreadIndex();

		// accumulated tasks
		HConcurrentQueue<shared_ptr<HBaseGraphTask>> Tasks;	

		// task thread base
		HArray<shared_ptr<HThreadRunnable>> TaskThreads;

		// named threads
		HArray<shared_ptr<HThreadRunnable>> NamedThreads;

		// task thread base id generator
		HThreadSafeCounter TaskThreadIdGenerator;	

		// task thread local storage singleton id generator
		HThreadSafeCounter TaskThreadLocalStorageIdGenerator;
	};

	class HTaskThreadLocalStorage
	{
	public:
		HTaskThreadLocalStorage() {}

		virtual ~HTaskThreadLocalStorage() {}

		// unique task thread id
		uint32 TaskThreadId;
	};

	class HTaskThreadBase : public HThreadRunnable
	{
	public:
		HTaskThreadBase();
		virtual ~HTaskThreadBase();

		virtual void Run() override;
		virtual void Terminate() override;

		// common method
		bool IsTerminated() { return bTerminate.GetValue() != 0; }

		template <typename CallableType>
		shared_ptr<HTaskThreadLocalStorage> GetTTLS(uint32 Index, CallableType&& InCallable);

		// shared task-thread context
		static HTaskThreadSharedContext TaskThreadSharedContext;

	protected:
		bool IsValidTTLSIndex(uint32 Index);

		// member variables
		HArray<shared_ptr<HTaskThreadLocalStorage>> TaskThreadLocalStorages;

		// generated from HTaskThreadSharedContext::
		uint32 UniqueId;

		HThreadSafeCounter bTerminate;
	};

	class HNamedTaskThread : public HTaskThreadBase
	{
	public:
		HNamedTaskThread(const HString& InName);
		virtual ~HNamedTaskThread();

		virtual void Run() final;

		// enqueue the task
		void EnqueueGraphTaskToOwnQueue(shared_ptr<HBaseGraphTask> InTask);

	protected:
		HConcurrentQueue<shared_ptr<HBaseGraphTask>> Tasks;
	};

	template <class DataType>
	class HTaskThreadSingleton : public HTaskThreadLocalStorage
	{
	public:
		HTaskThreadSingleton(unique_ptr<DataType>& InData)
			: HTaskThreadLocalStorage()
			, Data(HMove(InData))
		{}
		virtual ~HTaskThreadSingleton() {}

		static HCriticalSection AllocTTLSSyncObject;
		static DataType* Get() 		
		{
			static uint32 TaskThreadLSIndex = -1;
			if (TaskThreadLSIndex == -1)
			{
				// allocate ttls(task thread local storage); only call once
				HScopedLock Lock(AllocTTLSSyncObject);
				if (TaskThreadLSIndex == -1)
				{
					TaskThreadLSIndex = HTaskThreadBase::TaskThreadSharedContext.AllocTaskThreadLS();
				}				
			}

			// get the ttls
			HTaskThreadBase* TaskThread = (HTaskThreadBase*)(HTaskThreadBase::TaskThreadSharedContext.GetTaskThread().get());
			if (TaskThread == nullptr)
			{
				// try to find a named thread
				TaskThread = (HTaskThreadBase*)(HTaskThreadBase::TaskThreadSharedContext.GetNamedThread().get());
				if (TaskThread == nullptr)
				{
					// no valid running task thread exists
					return nullptr;
				}				
			}

			shared_ptr<HTaskThreadLocalStorage> TTLSInstance = TaskThread->GetTTLS(TaskThreadLSIndex, []()->shared_ptr<HTaskThreadLocalStorage>
			{
				unique_ptr<DataType> NewData = make_unique<DataType>();
				return make_shared<HTaskThreadLocalStorage, HTaskThreadSingleton<DataType>>(NewData);
			});

			return (((HTaskThreadSingleton<DataType>*)(TTLSInstance.get()))->Data).get();
		}

	protected:
		// thread local storage data
		unique_ptr<DataType> Data;
	};

	template <typename CallableType>
	shared_ptr<HTaskThreadLocalStorage> HTaskThreadBase::GetTTLS(uint32 Index, CallableType&& InCallable)
	{
		if (!IsValidTTLSIndex(Index))
		{
			// allocate ttls
			if (TaskThreadLocalStorages.size() <= Index)
			{
				TaskThreadLocalStorages.resize(Index + 1);
			}
			TaskThreadLocalStorages[Index] = InCallable();
		}
		return TaskThreadLocalStorages[Index];
	}

	template <class DataType>
	HCriticalSection HTaskThreadSingleton<DataType>::AllocTTLSSyncObject;
} }