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

	protected:
		int32 GetTaskThreadIndex();

		// accumulated tasks
		HConcurrentQueue<shared_ptr<HBaseGraphTask>> Tasks;	

		// task thread base
		HArray<shared_ptr<HThreadRunnable>> TaskThreads;

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
	};

	template <class DataType>
	class HTaskThreadSingleton : public HTaskThreadLocalStorage
	{
	public:
		HTaskThreadSingleton()
			: HTaskThreadLocalStorage()
		{}
		virtual ~HTaskThreadSingleton() {}

		static DataType* Get() 		
		{
			static uint32 TaskThreadLSIndex = -1;
			if (TaskThreadLSIndex == -1)
			{
				// allocate ttls(task thread local storage); only call once
				uint32& RefTaskThreadLSIndex = TaskThreadLSIndex;
				HCallOnceLock Lock(SyncObject, [&RefTaskThreadLSIndex]()
				{					
					RefTaskThreadLSIndex = HTaskThreadBase::TaskThreadSharedContext.AllocTaskThreadLS();
				});
			}

			// get the ttls
			HTaskThreadBase* TaskThread = (HTaskThreadBase*)(HTaskThreadBase::TaskThreadSharedContext.GetTaskThread().get());
			if (TaskThread == nullptr)
			{
				// no valid running task thread exists
				return nullptr;
			}

			shared_ptr<HTaskThreadLocalStorage> TTLSInstance = TaskThread->GetTTLS(TaskThreadLSIndex, []()->shared_ptr<HTaskThreadLocalStorage>
			{
				return make_shared<HTaskThreadLocalStorage, HTaskThreadSingleton<DataType>>();
			});

			return &(((HTaskThreadSingleton<DataType>*)(TTLSInstance.get()))->Data);
		}

	protected:
		// thread local storage data
		DataType Data;

		// call_once flag (static methods)
		static HCriticalSectionCallOnce SyncObject;
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
	HCriticalSectionCallOnce HTaskThreadSingleton<DataType>::SyncObject;
} }