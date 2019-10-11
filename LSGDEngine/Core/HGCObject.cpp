#include "HCorePCH.h"
#include "HGCObject.h"

using namespace lsgd;

IMPLEMENT_CLASS_TYPE1(HGCObjectReferencer, HObject)

void HGCObjectReferencer::Reflect()
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField("ReferencedObjects", &HGCObjectReferencer::ReferencedObjects);
}

void HGCObjectReferencer::AddObject(HGCObject* InObject)
{
	ReferencedObjects.push_back(InObject)
}

void HGCObjectReferencer::RemoveObject(HGCObject* InObject)
{
	int32 RemoveOffset = 0;
	int32 Offset = 0;
	for (auto& ReferencedObject : ReferencedObjects)
	{
		if (ReferencedObject == InObject)
		{
			RemoveOffset = 
		}
		RemoveOffset++;
	}

	ReferencedObjects.erase()
}