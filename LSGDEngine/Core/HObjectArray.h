#pragma once

#include "HObject.h"

using namespace lsgd::container;

namespace lsgd
{
	enum EObjectItemFlags : uint64
	{
		MarkAsDestroyed = 1 << 0,
	};

	class HObjectItem
	{
	public:
		virtual ~HObjectItem() {}

	protected:
		// the only class which can access
		friend class HObjectArray;

		HObjectItem();

		void Bind(unique_ptr<HObject>& InObject, uint64 InFlag);
		void Unbind();

		void SetFlag(uint64 InFlags);
		void UnsetFlag(uint64 InFlags);
		bool HasFlags(EObjectItemFlags InFlag);

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

		bool IsValidObject(uint32 Index, uint32 SerialNumber);

		// mark proper flags
		void SetAsDestroyed(uint32 Index, uint32 SerialNumber);
		
	protected:
		friend class HSingletonTemplate<HObjectArray>;
		HObjectArray();

		void InitializeObjectArray();

		// make it only allowed in GC
		void DeregisterObject(uint32 Index, uint32 SerialNumber);

		// object array
		HObjectArrayType Objects;
		// free indices
		HConcurrentQueue<uint32> FreeIndices;

		// registered observers (create/delete)
		HArray<HObjectCreateListener*> ObjectCreateListeners;
		HArray<HObjectDeleteListener*> ObjectDeleteListeners;
	};
}