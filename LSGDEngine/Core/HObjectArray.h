#pragma once

#include "HObject.h"

using namespace lsgd::container;

namespace lsgd { namespace gc {

	class HGCSweepExecutor;

} }

namespace lsgd
{
	enum EObjectItemFlags : uint64
	{
		MarkAsDestroyed = 1 << 0,
		RootSet			= 1 << 1,
		MarkGC			= 1 << 2,
	};

	class HObjectItem
	{
	public:
		virtual ~HObjectItem() {}

	protected:
		// the only class which can access
		friend class HObjectArray;
		friend class gc::HGCSweepExecutor;

		HObjectItem();

		void Bind(unique_ptr<HObject>& InObject, uint64 InFlag, uint32 InIndex);
		void Unbind();

		void SetFlag(uint64 InFlags);
		void UnsetFlag(uint64 InFlags);
		bool HasFlags(EObjectItemFlags InFlag);

		bool IsEmpty() const { return UniqueNumber == -1; }

		unique_ptr<HObject> Object;
		int64 Flags;
		uint32 UniqueNumber;

		static HThreadSafeCounter UniqueNumberGenerator;
	};

#define DefaultMaxObjectNum (10 * 1024)
	typedef HFixedArray<unique_ptr<HObjectItem>, DefaultMaxObjectNum> HObjectArrayType;
	
	class HObjectArray : public HSingletonTemplate<HObjectArray>
	{
	public:		
		~HObjectArray(){}

		// object array's create/delete listeners
		class HObjectCreateListener
		{
		public:
			HObjectCreateListener();
			~HObjectCreateListener();
		};

		class HObjectDeleteListener
		{
		public:
			HObjectDeleteListener();
			~HObjectDeleteListener();
		};

		HObjectArrayData RegisterObject(unique_ptr<HObject>& InObject, int64 InFlags);
		HObject* GetObject(uint32 Index, uint32 SerialNumber);
		HArray<HObject*> GetRootSetObjects();

		bool IsValidObject(uint32 Index, uint32 SerialNumber);

		// mark proper flags
		void SetAsDestroyed(uint32 Index, uint32 SerialNumber);
		void SetAsRootSet(uint32 Index, uint32 SerialNumber);
		void MarkGC(uint32 Index, uint32 SerialNumber);
		void UnMarkGC(uint32 Index, uint32 SerialNumber);
		
	protected:
		friend class HSingletonTemplate<HObjectArray>;
		friend class gc::HGCSweepExecutor;

		HObjectArray();

		void InitializeObjectArray();

		// make it only allowed in GC
		void DeregisterObject(uint32 Index, uint32 SerialNumber);

		// object array
		HObjectArrayType Objects;
		// free indices
		HConcurrentQueue<uint32> FreeIndices;
		// root set object array
		HArray<uint32> RootSetObjects;

		// registered observers (create/delete)
		HArray<HObjectCreateListener*> ObjectCreateListeners;
		HArray<HObjectDeleteListener*> ObjectDeleteListeners;
	};
}