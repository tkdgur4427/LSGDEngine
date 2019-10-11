#include "HCorePCH.h"
#include "HGCObject.h"

using namespace lsgd;

IMPLEMENT_CLASS_TYPE1(HGCObjectReferencer, HObject)

void HGCObjectReferencer::Reflect()
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField("ReferencedObjects", &HGCObjectReferencer::ReferencedObjects);
}

void HGCObjectReferencer::AddReferencedObjects(HObjectHandleWeak<HObject> InObject, class HReferenceCollector& Collector)
{

}

void HGCObjectReferencer::AddObject(HGCObject* InObject)
{
	ReferencedObjects.push_back(InObject)
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