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

void HObjectItem::Bind(unique_ptr<HObject>& InObject, uint64 InFlag)
{
	check(Flags == 0);

	Flags = InFlag;
	UniqueNumber = UniqueNumberGenerator.Increment();

	// move the object real instance to the slot
	Object = HMove(InObject);
}

void HObjectItem::Unbind()
{
	// reset the unique number and flags
	Flags = 0;
	UniqueNumber = -1;

	// release the Object binded in this item slot
	Object.reset();
}

void HObjectItem::SetFlag(uint64 InFlags)
{
	Flags |= InFlags;
}

void HObjectItem::UnsetFlag(uint64 InFlags)
{
	Flags &= ((uint64)(-1) ^ InFlags);
}

bool HObjectItem::HasFlags(EObjectItemFlags InFlag)
{
	return (Flags & InFlag) != 0;
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

HObjectArrayData HObjectArray::RegisterObject(unique_ptr<HObject>& InObject, int64 InFlags)
{
	uint32 NewIndex = FreeIndices.Pop();
	Objects[NewIndex]->Bind(InObject, InFlags);

	return {Objects[NewIndex]->UniqueNumber, NewIndex};
}

void HObjectArray::DeregisterObject(uint32 Index, uint32 SerialNumber)
{
	HObjectItem* ObjectItem = Objects[Index].get();
	check(ObjectItem->UniqueNumber == SerialNumber);
	
	// release the object
	ObjectItem->Unbind();

	// add to free index
	FreeIndices.Push(Index);
}

bool HObjectArray::IsValidObject(uint32 Index, uint32 SerialNumber)
{
	HObjectItem* ObjectItem = Objects[Index].get();
	if (ObjectItem->HasFlags(EObjectItemFlags::MarkAsDestroyed))
	{
		return false;
	}

	if (ObjectItem->UniqueNumber != SerialNumber)
	{
		return false;
	}

	return true;
}

HObject* HObjectArray::GetObject(uint32 Index, uint32 SerialNumber)
{
	if (!IsValidObject(Index, SerialNumber))
	{
		return nullptr;
	}	
	
	HObjectItem* ObjectItem = Objects[Index].get();	
	return ObjectItem->Object.get();
}

void HObjectArray::SetAsDestroyed(uint32 Index, uint32 SerialNumber)
{	
	check(IsValidObject(Index, SerialNumber));

	HObjectItem* ObjectItem = Objects[Index].get();
	ObjectItem->SetFlag(EObjectItemFlags::MarkAsDestroyed);
}