#pragma once

// HTypeDatabaseUtil
#include "HTypeDatabaseUtils.h"

// forward declarations
namespace lsgd { namespace reflect {
	class HClass;
	class HReflectionContext;
} }

namespace lsgd
{
	// forward declarations
	class HObject;
	class HPackage;

	class HObjectInitializer
	{
	public:
		HObjectInitializer();

		void Reset();
				
		HObject* Object;
		const reflect::HClass* Class;

		HPackage* Package;
		
		// validation attributes
		uint32 TotalSize;
		uint32 RealSize;
	};

	enum EObjectFlags
	{
		TagExp = (1 << 0),
		TagImp = (1 << 1),
	};

	struct HCoreObjectState
	{
		HCoreObjectState()
			: ObjectFlags(0)
		{}

		void TagObjectFlags(EObjectFlags InValue);
		void UntagObjectFlags(EObjectFlags InValue);

		// bit flags for EObjectFlags
		int64 ObjectFlags;
	};

	class HCoreObject
	{
	public:
		HCoreObject();

	//protected:
		void GenerateName();

		// object's unique name
		HName Name;
		// class definition
		const reflect::HClass* Class;

		// object state
		HCoreObjectState State;
	};

	//@todo - need to make separate thread local storage; for now, just temporary
	extern thread_local HObjectInitializer LObjectInitializer;

	extern HObject* AllocateHObjectInternal(HObjectInitializer& ObjectInitializer, const reflect::HClass* InClass);
	
	class HObject : public HCoreObject
	{
	public:
		HObject(HObjectInitializer& InObjectInitializer = LObjectInitializer);

		// virtual methods
		virtual void Serialize(reflect::HReflectionContext& InContext);

		// outer object
		class HPackage* Package;
	};	

	// object array data (common data)
	struct HObjectArrayData
	{
		void Reset()
		{
			SerialNumber = -1;
			Index = -1;
		}

		uint32 SerialNumber;	// object's unique number
		uint32 Index;			// GObjectArray's index
	};

	// object helper class 
	struct HObjectHelper
	{
		static HObject* GetObject(uint32 Index, uint32 SerialNumber);
		static void SetAsDestroyed(const HObjectArrayData& InData);
	};

	// object handle	
	class HObjectHandle
	{
	public:
		// whether the handle has ownership of a object
		bool IsOwned() const { return bIsOwned; }
		// whether object handle is valid or not
		bool IsValid() const { return HObjectHelper::GetObject(Data.Index, Data.SerialNumber) != nullptr; }
		
		// get the object
		template <typename ObjectType>
		ObjectType* GetObject()
		{
			// early validation out
			if (Data.Index == -1)
			{
				return nullptr;
			}

			// @todo - HCast<>
			return (ObjectType*)HObjectHelper::GetObject(Data.Index, Data.SerialNumber);
		}

	protected:
		HObjectHandle(bool InbIsOwned)
			: bIsOwned(InbIsOwned)
		{
			Data.Reset();
		}

		HObjectHandle(const HObjectArrayData& InData, bool InbIsOwned)
			: Data(InData)
			, bIsOwned(InbIsOwned)
		{}		

		HObjectArrayData Data;
		bool bIsOwned;
	};

	template <typename ObjectType>
	class HObjectHandleUnique : public HObjectHandle
	{
	public:
		HObjectHandleUnique()
			: HObjectHandle(true)
		{}

		// explicit constructor
		explicit HObjectHandleUnique(const HObjectArrayData& InData)
			: HObjectHandle(InData, true)
		{
			check(IsValid()); // for explicit constructor, object should be valid
		}

		~HObjectHandleUnique()
		{
			HObjectHelper::SetAsDestroyed(Data);
		}

		// constructor/assignment that allows move semantics
		HObjectHandleUnique(HObjectHandleUnique&& Moving) noexcept
		{
			Moving.Swap(*this);
		}

		HObjectHandleUnique& operator=(HObjectHandleUnique&& Moving) noexcept
		{
			HObjectHandleUnique<T> Tmp(Moving.Release());
			Tmp.Swap(*this);
		}

		HObjectHandleUnique(HObjectHandleUnique const&) = delete;
		HObjectHandleUnique& operator=(HObjectHandleUnique const&) = delete;

		ObjectType* operator->() const { return GetObject<ObjectType>(); }
		ObjectType& operator*() const { return *GetObject<ObjectType>(); }
		ObjectType* Get() const { return GetObject<ObjectType>(); }
		explicit operator bool() const { return (Get() != nullptr); }

		HObjectArrayData Release() noexcept
		{
			HObjectArrayData Result;
			HSwap(Result, Data);
			return Result;
		}

		void Swap(HObjectHandleUnique& InUnqiueHandle) noexcept
		{
			HSwap(Data, InUnqiueHandle.Data);
		}

		void Reset()
		{
			HObjectArrayData Tmp = Release();
			HObjectHelper::SetAsDestroyed(Tmp);
		}
	};
	
	template <typename ObjectType>
	class HObjectHandleWeak : public HObjectHandle
	{
	public:
		HObjectHandleWeak()
			: HObjectHandle(false)
		{}
	};

	// forward declaration
	extern class HPackage* GTransientPackage;

	extern HObjectArrayData AllocateHObjectInner(const reflect::HClass* InClass, class HPackage* InPackage);

	extern HObjectArrayData AllocateHObject(const HString& ClassName, class HPackage* InPackage = nullptr);

	template <typename ObjectType>
	HObjectHandleUnique<ObjectType> AllocateHObject(class HPackage* InPackage = nullptr)
	{
		// get the HClass type with template parameter, HObjectType
		reflect::HTypeDescriptor ClassType = reflect::HTypeDatabaseUtils::GetTypeDescriptor<ObjectType>();
		check(ClassType.ClassType != nullptr);

		// get HClass
		const reflect::HClass* Class = ClassType.ClassType;

		// @todo - need to dynamic RTTI checking for whether this class is derived or not
		HObjectArrayData NewData = AllocateHObjectInner(Class, InPackage);
		return HObjectHandleUnique<ObjectType>(NewData.SerialNumber, NewData.Index);
	}
}