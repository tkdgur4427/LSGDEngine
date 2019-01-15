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

void HObjectItem::Bind(unique_ptr<HObject>& InObject, uint64 InFlag, uint32 InIndex)
{
	check(Flags == 0);

	Flags = InFlag;
	UniqueNumber = UniqueNumberGenerator.Increment();

	// move the object real instance to the slot
	Object = HMove(InObject);

	// set the object's state as binded object array data
	Object->State.ObjectArrayData = { UniqueNumber, InIndex };
}

void HObjectItem::Unbind()
{
	// reset the unique number and flags
	Flags = 0;
	UniqueNumber = -1;

	// reset the data
	Object->State.ObjectArrayData.Reset();

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
	uint32 NewIndex = -1;
	FreeIndices.TryPop(&NewIndex);
	check(NewIndex != -1);

	Objects[NewIndex]->Bind(InObject, InFlags, NewIndex);

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

HArray<HObject*> HObjectArray::GetRootSetObjects()
{
	HArray<HObject*> Result;
	Result.resize(RootSetObjects.size());

	int32 Index = 0;
	for (auto& RootSetIndex : RootSetObjects)
	{
		Result[Index++] = Objects[RootSetIndex]->Object.get();
	}

	return Result;
}

void HObjectArray::SetAsDestroyed(uint32 Index, uint32 SerialNumber)
{	
	check(IsValidObject(Index, SerialNumber));

	HObjectItem* ObjectItem = Objects[Index].get();
	ObjectItem->SetFlag(EObjectItemFlags::MarkAsDestroyed);
}

void HObjectArray::SetAsRootSet(uint32 Index, uint32 SerialNumber)
{
	check(IsValidObject(Index, SerialNumber));

	HObjectItem* ObjectItem = Objects[Index].get();
	ObjectItem->SetFlag(EObjectItemFlags::RootSet);

	// add to fast look-up table for rootset
	RootSetObjects.push_back(Index);
}

void HObjectArray::UnsetAsRootSet(uint32 Index, uint32 SerialNumber)
{
	check(IsValidObject(Index, SerialNumber));

	HObjectItem* ObjectItem = Objects[Index].get();
	ObjectItem->UnsetFlag(EObjectItemFlags::RootSet);

	// remove fast look-up table for rootset
	uint32 Offset = 0;
	uint32 FoundOffset = -1;
	for (auto& RootSetObject : RootSetObjects)
	{
		if (RootSetObject == Index)
		{
			FoundOffset = Offset;
			break;
		}
		Offset++;
	}
	check(FoundOffset != -1);
	RootSetObjects.erase(RootSetObjects.begin() + FoundOffset);
}

void HObjectArray::MarkGC(uint32 Index, uint32 SerialNumber)
{
	check(IsValidObject(Index, SerialNumber));

	HObjectItem* ObjectItem = Objects[Index].get();
	ObjectItem->SetFlag(EObjectItemFlags::MarkGC);
}

void HObjectArray::UnMarkGC(uint32 Index, uint32 SerialNumber)
{
	check(IsValidObject(Index, SerialNumber));

	HObjectItem* ObjectItem = Objects[Index].get();
	ObjectItem->UnsetFlag(EObjectItemFlags::MarkGC);
}