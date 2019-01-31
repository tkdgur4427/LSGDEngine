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
			HScopedLock Lock(SyncObject);
			return &LinkedList;
		}

		static void AddToGlobalList(Type* InData)
		{
			HScopedLock Lock(SyncObject);
			LinkedList.push_back(InData);
		}

		static void RemvoeFromGlobalList(Type* InData)
		{
			HScopedLock Lock(SyncObject);
			LinkedList.remove(InData);
		}

		static HList<Type*> LinkedList;
		static HCriticalSection SyncObject;
	};
}