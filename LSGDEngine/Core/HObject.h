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
		reflect::HClass* Class;

		HPackage* Package;
		
		// validation attributes
		uint32 TotalSize;
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
		reflect::HClass* Class;
	};

	//@todo - need to make separate thread local storage; for now, just temporary
	extern thread_local HObjectInitializer LObjectInitializer;

	extern HObject* AllocateHObjectInternal(HObjectInitializer& ObjectInitializer, reflect::HClass* InClass);
	
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
	extern class HPackage* GTranscientPackage;

	template <typename HObjectType>
	HObjectType* AllocateHObject(class HPackage* InPackage = nullptr)
	{
		// get the HClass type with template parameter, HObjectType
		reflect::HTypeDescriptor ClassType = reflect::HTypeDatabaseUtils::GetTypeDescriptor<HObjectType>();
		check(ClassType.ClassType != nullptr);

		// set the package
		LObjectInitializer.Package = InPackage;

		// when there is no package, set the GTranscientPackage
		if (LObjectInitializer.Package == nullptr)
		{
			LObjectInitializer.Package = GTransientPackage;
		}

		// try to allocate HObject
		HObject* NewObject = AllocateHObjectInternal(LObjectInitializer, ClassType.ClassType, InPackage);
		check(NewObject != nullptr);
		check(LObjectInitializer.TotalSize == sizeof(HObjectType));
		check(LObjectInitializer.Object == NewObject);

		// trigger constructor
		// @todo : need to support arbitrary number of constructor parameters forwarding
		new (NewObject) HObjectType();		

		// set the class to NewObject
		NewObject->Class = LObjectInitializer.Class;

		// generate unique object name
		NewObject->GenerateName();

		// reset the ObjectInitializer
		LObjectInitializer.Reset();

		// @todo - need to dynamic RTTI checking for whether this class is derived or not
		return (HObjectType*)NewObject;
	}
}