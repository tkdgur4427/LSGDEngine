#include "HCorePCH.h"
#include "HGCObject.h"

// HCast
#include "HCastType.h"
#include "HReflectImplement.h"

using namespace lsgd;

HObjectHandleUnique<HGCObjectReferencer> HGCObject::GCObjectReferencer;

IMPLEMENT_CLASS_TYPE1(HGCObjectReferencer, HObject)

void HGCObjectReferencer::Reflect()
{
	
}

void HGCObjectReferencer::AddReferencedObjects(HObject* InObject, gc::HReferenceCollector& Collector)
{
	HGCObjectReferencer* ObjectReferencer = HCast<HObject, HGCObjectReferencer>(InObject);
	if (ObjectReferencer != nullptr)
	{
		for (HGCObject* Object : ObjectReferencer->ReferencedObjects)
		{
			Object->AddReferencedObjects(Collector);
		}
	}
}

void HGCObjectReferencer::AddObject(HGCObject* InObject)
{
	ReferencedObjects.push_back(InObject);
}

void HGCObjectReferencer::RemoveObject(HGCObject* InObject)
{
	// find the offset to remove referenced object
	int32 RemoveOffset = -1;
	int32 Offset = 0;
	for (auto& ReferencedObject : ReferencedObjects)
	{
		if (ReferencedObject == InObject)
		{
			RemoveOffset = Offset;
		}
		Offset++;
	}
	check(RemoveOffset != -1);

	// remove the offset
	ReferencedObjects.erase(ReferencedObjects.begin() + RemoveOffset);
}