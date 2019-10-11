#pragma once

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HGCObjectReferencer, HObject)
	class HGCObjectReferencer : public HObject
	{
	public:
		GENERATE_CLASS_BODY(HGCObjectReferencer);

		HGCObjectReferencer() {}
		virtual ~HGCObjectReferencer() {}

		void AddObject(class HGCObject* InObject);
		void RemoveObject(class HGCObject* InObject);

		HArray<HGCObject*> ReferencedObjects;
		HCriticalSection ReferencedObjectsCritical;
	};

	class HGCObject
	{
	public:
		bool bReferencedAdded = false;

		void Init()
		{
			
		}

		static void StaticInit()
		{
			if (!GCObjectReferencer.IsValid())
			{
				GCObjectReferencer = AllocateHObject(HGCObjectReferencer::GetClassName());
				GCObjectReferencer.SetRoot();
			}
		}

		static HObjectHandleUnique<HGCObjectReferencer> GCObjectReferencer;
	};
}