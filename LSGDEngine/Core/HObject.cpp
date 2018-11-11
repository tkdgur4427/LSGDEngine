#include "HCorePCH.h"
#include "HObject.h"

// HClass
#include "HReflect.h"

using namespace lsgd;
using namespace lsgd::reflect;

thread_local lsgd::HObjectInitializer lsgd::LObjectInitializer;

HObjectInitializer::HObjectInitializer()
	: Object(nullptr)
	, Class(nullptr)
	, TotalSize(0)
	, Package(nullptr)
{

}

void HObjectInitializer::Reset()
{
	Package = nullptr;
}

HCoreObject::HCoreObject()
{

}

void HCoreObject::GenerateName()
{
	check(Class != nullptr);
	
	// generate unique name
	Name = HName(Class->Name.c_str());
}

void HCoreObjectState::TagObjectFlags(EObjectFlags InValue)
{
	ObjectFlags |= InValue;
}

void HCoreObjectState::UntagObjectFlags(EObjectFlags InValue)
{
	ObjectFlags &= (0llu ^ ~(InValue));
}

HObject::HObject(HObjectInitializer& InObjectInitializer)
	: HCoreObject()
	, Package(InObjectInitializer.Package)
{
	// setting current initialized object as this object
	InObjectInitializer.Object = this;
}

void HObject::Serialize(reflect::HReflectionContext& InContext)
{
	check(Class != nullptr);
	Class->SerializeProperties(InContext, (uint8*)this);
}

#include "HPackage.h"

namespace lsgd
{
	HObject* AllocateHObject(const HString& ClassName, class HPackage* InPackage)
	{
		const HClass* Class = reflect::HTypeDatabaseUtils::GetTypeDescriptor(ClassName).ClassType;
		return AllocateHObjectInner(Class, InPackage);
	}

	HObject* AllocateHObjectInner(const reflect::HClass* InClass, class HPackage* InPackage)
	{
		// get the class name
		const HString& ClassName = InClass->Name;
		// get the common type helper
		HCommonTypeHelperInterface* CommonTypeHelper = const_cast<HCommonTypeHelperInterface*>(reflect::HTypeDatabaseUtils::GetClassCommonTypeHelper(ClassName));

		// set the package
		LObjectInitializer.Package = InPackage;

		// when there is no package, set the GTranscientPackage
		if (LObjectInitializer.Package == nullptr)
		{
			LObjectInitializer.Package = GTransientPackage;
		}

		// set the real class size
		LObjectInitializer.RealSize = CommonTypeHelper->GetSize();

		// try to allocate HObject
		HObject* NewObject = AllocateHObjectInternal(LObjectInitializer, InClass);
		check(NewObject != nullptr);
		//check(LObjectInitializer.TotalSize == sizeof(HObjectType));
		check(LObjectInitializer.Object == NewObject);

		// trigger constructor
		// @todo : need to support arbitrary number of constructor parameters forwarding
		CommonTypeHelper->TriggerConstructor(NewObject);

		// set the class to NewObject
		NewObject->Class = LObjectInitializer.Class;

		// generate unique object name
		NewObject->GenerateName();

		// reset the ObjectInitializer
		LObjectInitializer.Reset();

		return NewObject;
	}

	HObject* AllocateHObjectInternal(HObjectInitializer& ObjectInitializer, const reflect::HClass* InClass)
	{
		uint32 SizeOfHObject = sizeof(HObject);
		uint32 SizeOfHClass = InClass->GetTotalSize();
		ObjectInitializer.TotalSize = SizeOfHClass + SizeOfHObject;

		// set the HClass in ObjectInitializer
		ObjectInitializer.Class = InClass;

		// note that we didn't trigger any class constructor, just naive allocation happens
		HObject* AllocatedObject = new (HGenericMemory::Allocate(ObjectInitializer.RealSize))HObject(ObjectInitializer);

		// @todo : remove! - temporary!
		if (AllocatedObject->Package == GTransientPackage)
		{
			// add tracked objects
			((HTranscientPackageForTest*)(AllocatedObject->Package))->TrackedObjects.push_back(AllocatedObject);
		}

		return AllocatedObject;
	}
}
