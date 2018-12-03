#pragma once

#include "HObject.h"

using namespace lsgd::container;

namespace lsgd
{
	class HObjectItem
	{
	protected:
		// the only class which can access
		friend class HObjectArray;

		HObjectItem();
		~HObjectItem() {}

		void Bind(unique_ptr<HObject>& InObject, int64 InFlag);
		void Unbind();		

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

		void RegisterObject(unique_ptr<HObject>& InObject);
		void DeregisterObject();
		
	protected:
		HObjectArray();

		void InitializeObjectArray();

		// object array
		HObjectArrayType Objects;
		// free indices
		HConcurrentQueue<uint32> FreeIndices;

		// registered observers (create/delete)
		HArray<HObjectCreateListener*> ObjectCreateListeners;
		HArray<HObjectDeleteListener*> ObjectDeleteListeners;
	};
}