#include "HCorePCH.h"
#include "HReferenceCollector.h"
#include "HObject.h"

// GCObject
#include "HGCObject.h"

#include "HProfiler.h"

using namespace lsgd;
using namespace lsgd::reflect;
using namespace lsgd::gc;

#define ENABLE_PROFILE_SERIALIZE_HOBJECT 0

HReflectionContext& HReferenceCollector::operator<<(HObject*& Value)
{
	// mark it for gc (reference collector)
	HObjectHelper::MarkGC(Value->GetObjectArrayData());

	// serialize the object
	{
#if ENABLE_PROFILE_SERIALIZE_HOBJECT
		HString NameString = Value->Name.ToString();
		SGD_COPED_PIX_PROFILER_VARS("Serialize_%s", NameString.c_str());
#endif
		Value->Serialize(*this);
	}
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