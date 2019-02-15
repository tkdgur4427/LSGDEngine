#pragma once

using namespace lsgd::thread;

namespace lsgd
{
	template <typename Type>
	class HGlobalLinkedList
	{
	public:
		static HList<Type*>* GetList()
		{
			HScopedLock Lock(GetSyncObject());
			static HList<Type*> LinkedList;
			return &LinkedList;
		}

		static HCriticalSection& GetSyncObject()
		{
			static HCriticalSection SyncObject;
			return SyncObject;
		}

		static void AddToGlobalList(Type* InData)
		{
			HScopedLock Lock(GetSyncObject());
			GetList()->push_back(InData);
		}

		static void RemvoeFromGlobalList(Type* InData)
		{
			HScopedLock Lock(GetSyncObject());
			GetList()->remove(InData);
		}
	};
}