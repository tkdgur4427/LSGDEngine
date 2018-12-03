#include "HCorePCH.h"
#include "HObjectArray.h"

using namespace lsgd;

// static definition
HThreadSafeCounter HObjectItem::UniqueNumberGenerator = 0;

HObjectItem::HObjectItem()
	: Flags(-1)
	, UniqueNumber(-1)
{

}

void HObjectItem::Bind(unique_ptr<HObject>& InObject, int64 InFlag)
{
	Flags = InFlag;
	UniqueNumber = UniqueNumberGenerator.Increment();

	// move the object real instance to the slot
	Object = move(InObject);
}

void HObjectItem::Unbind()
{
	// reset the unique number and flags
	Flags = -1;
	UniqueNumber = -1;

	// release the Object binded in this item slot
	Object.reset();
}

HObjectArray::HObjectCreateListener::HObjectCreateListener()
{
	HObjectArray::GetSingleton()->ObjectCreateListeners.push_back(this);
}

HObjectArray::HObjectCreateListener::~HObjectCreateListener()
{
	HArray<HObjectCreateListener*> RefListeners = HObjectArray::GetSingleton()->ObjectCreateListeners;

	uint32 RemoveLocation = -1;
	for (uint32 ListenerIndex = 0; ListenerIndex < RefListeners.size(); ++ListenerIndex)
	{
		if (RefListeners[ListenerIndex] != this)
		{
			RemoveLocation = ListenerIndex;
		}
	}
	check(RemoveLocation != -1);
	RefListeners.erase(RefListeners.begin() + RemoveLocation);
}

HObjectArray::HObjectDeleteListener::HObjectDeleteListener()
{
	HObjectArray::GetSingleton()->ObjectDeleteListeners.push_back(this);
}

HObjectArray::HObjectDeleteListener::~HObjectDeleteListener()
{
	HArray<HObjectDeleteListener*> RefListeners = HObjectArray::GetSingleton()->ObjectDeleteListeners;

	uint32 RemoveLocation = -1;
	for (uint32 ListenerIndex = 0; ListenerIndex < RefListeners.size(); ++ListenerIndex)
	{
		if (RefListeners[ListenerIndex] != this)
		{
			RemoveLocation = ListenerIndex;
		}
	}
	check(RemoveLocation != -1);
	RefListeners.erase(RefListeners.begin() + RemoveLocation);
}

HObjectArray::HObjectArray()
	: HSingletonTemplate()
{
	InitializeObjectArray();
}

void HObjectArray::InitializeObjectArray()
{
	uint32 Index = 0;
	for (auto&& ObjectItem : Objects)
	{
		ObjectItem->Unbind();
		FreeIndices.Push(Index);
		Index++;
	}
}