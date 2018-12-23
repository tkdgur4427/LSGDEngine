#pragma once

// HTypeDatabaseUtil
#include "HTypeDatabaseUtils.h"

#include "HObjectHandle.h"

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

		// object array data
		HObjectArrayData ObjectArrayData;
	};

	class HCoreObject
	{
	public:
		HCoreObject();

		// get the object array data
		const HObjectArrayData& GetObjectArrayData() const { return State.ObjectArrayData; }

		// IsA (RTTI)
		template <class To>
		bool IsA()
		{
			bool bResult = false;

			// get the class descriptor
			reflect::HTypeDescriptor TypeDesc = reflect::HTypeDatabaseUtils::GetTypeDescriptor<To>();
			check(TypeDesc.ClassType != nullptr);
			
			const reflect::HStruct* SuperClass = Class;
			while (SuperClass != nullptr)
			{
				if (SuperClass == TypeDesc.ClassType)
				{
					bResult = true;
					break;
				}

				SuperClass = SuperClass->SuperStruct;
			}

			return true;
		}

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
		return HObjectHandleUnique<ObjectType>(NewData);
	}
}