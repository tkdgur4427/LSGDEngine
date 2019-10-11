#pragma once

#include "HObject.h"
#include "HReferenceCollector.h"

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HGCObjectReferencer, HObject)
	class HGCObjectReferencer : public HObject
	{
	public:
		GENERATE_CLASS_BODY(HGCObjectReferencer)

		HGCObjectReferencer() {}
		virtual ~HGCObjectReferencer() {}

		void AddObject(class HGCObject* InObject);
		void RemoveObject(class HGCObject* InObject);

		static void AddReferencedObjects(HObject* InObject, gc::HReferenceCollector& Collector);

		HArray<HGCObject*> ReferencedObjects;
		HCriticalSection ReferencedObjectsCritical;
	};

	class HGCObject
	{
	public:
		bool bReferencedAdded = false;

		void Init()
		{
			StaticInit();
			check(GCObjectReferencer.IsValid());

			// add this instance to the reference list
			GCObjectReferencer->AddObject(this);
			bReferencedAdded = true;
		}

		HGCObject()
		{
			Init();
		}

		virtual ~HGCObject()
		{
			if (GCObjectReferencer.IsValid() && bReferencedAdded)
			{
				GCObjectReferencer->RemoveObject(this);
			}
		}

		static void StaticInit()
		{
			if (!GCObjectReferencer.IsValid())
			{
				GCObjectReferencer = HMove(HObjectHandleUnique<HGCObjectReferencer>(AllocateHObject(HGCObjectReferencer::GetClassName())));
				GCObjectReferencer.SetRoot();
			}
		}

		virtual void AddReferencedObjects(gc::HReferenceCollector& Collector) = 0;

		static HObjectHandleUnique<HGCObjectReferencer> GCObjectReferencer;
	};
}