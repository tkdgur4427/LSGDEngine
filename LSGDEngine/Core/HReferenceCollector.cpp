#include "HCorePCH.h"
#include "HReferenceCollector.h"
#include "HObject.h"

// GCObject
#include "HGCObject.h"

using namespace lsgd;
using namespace lsgd::reflect;
using namespace lsgd::gc;

HReflectionContext& HReferenceCollector::operator<<(HObject*& Value)
{
	// mark it for gc (reference collector)
	HObjectHelper::MarkGC(Value->GetObjectArrayData());

	// serialize the object
	Value->Serialize(*this);
	return *this;
}

void HReferenceCollector::Collect()
{
	// marking for HGCObject
	if (HGCObject::GCObjectReferencer.IsValid())
	{
		// convert unique to weak object
		HGCObjectReferencer::AddReferencedObjects(HGCObject::GCObjectReferencer.Get(), *this);
	}

	// marking the objects
	for (auto& RootObject : RootSet)
	{
		(*this) << RootObject;
	}
}