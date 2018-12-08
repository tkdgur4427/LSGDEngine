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
		uint32 SerialNumber;	// object's unique number
		uint32 Index;			// GObjectArray's index
	};

	// object helper class 
	struct HObjectHelper
	{
		static HObject* GetObject(uint32 Index, uint32 SerialNumber);
	};

	// object handle
	template <typename ObjectType, bool bIsOwned>
	class HObjectHandle
	{
	public:
		// whether the handle has ownership of a object
		bool IsOwned() const { return bIsOwned; }
		
		// get the object
		ObjectType* GetObject()
		{
			return HObjectHelper::GetObject(Data.Index, Data.SerialNumber);
		}

	protected:
		HObjectHandle(uint32 InSerialNumber, uint32 InIndex)
		{
			Data.Index = InIndex;
			Data.SerialNumber = InSerialNumber;
		}

		HObjectArrayData Data;
	};

	template <typename ObjectType>
	class HObjectHandleUnique : public HObjectHandle<ObjectType, true>
	{
	public:
		explicit HObjectHandleUnique(const HObjectHandleUnique& InObjectHandle)
			: HObjectHandle(InObjectHandle.Data.SerialNumber, InObjectHandle.Data.Index)
		{}
		explicit HObjectHandleUnique(uint32 InSerialNumber, uint32 InIndex)
			: HObjectHandle(InSerialNumber, InIndex)
		{}
	};
	
	template <typename ObjectType>
	class HObjectHandleWeak : public HObjectHandle<ObjectType, false>
	{
	public:
		explicit HObjectHandleWeak(const HObjectHandleWeak& InObjectHandle)
			: HObjectHandle(InObjectHandle.Data.SerialNumber, InObjectHandle.Data.Index)
		{}
		explicit HObjectHandleWeak(uint32 InSerialNumber, uint32 InIndex)
			: HObjectHandle(InSerialNumber, InIndex)
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