#include "HCorePCH.h"
#include "HObject.h"

// HClass
#include "HReflect.h"
// HPackage
#include "HPackage.h"
// HObjectArray
#include "HObjectArray.h"
// core tls
#include "HCoreTls.h"
// reflection
#include "HReflectImplement.h"

using namespace lsgd;
using namespace lsgd::reflect;

thread_local lsgd::HObjectInitializer* lsgd::LObjectInitializer;

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

void HCoreObject::Reflect()
{
	// add name field
	HTypeDatabase::GetSingleton()->AddClassField("Name", &HCoreObject::Name);
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

namespace lsgd
{
	HObjectArrayData AllocateHObject(const HString& ClassName, class HPackage* InPackage)
	{
		const HClass* Class = reflect::HTypeDatabaseUtils::GetTypeDescriptor(ClassName).ClassType;
		return AllocateHObjectInner(Class, InPackage);
	}

	HObjectArrayData AllocateHObjectInner(const reflect::HClass* InClass, class HPackage* InPackage)
	{
		LObjectInitializer = &(HCoreTls::Get()->ObjectInitializer);

		// get the class name
		const HString& ClassName = InClass->Name;
		// get the common type helper
		HCommonTypeHelperInterface* CommonTypeHelper = const_cast<HCommonTypeHelperInterface*>(reflect::HTypeDatabaseUtils::GetClassCommonTypeHelper(ClassName));

		// set the package
		LObjectInitializer->Package = InPackage;

		// when there is no package, set the GTranscientPackage
		bool bNeedRootSet = false;
		if (LObjectInitializer->Package == nullptr)
		{
			LObjectInitializer->Package = GTransientPackage;

			// @todo - temporary
			bNeedRootSet = true;
		}

		// set the real class size
		LObjectInitializer->RealSize = CommonTypeHelper->GetSize();

		// try to allocate HObject
		HObject* NewObject = AllocateHObjectInternal(*LObjectInitializer, InClass);
		check(NewObject != nullptr);
		//check(LObjectInitializer.TotalSize == sizeof(HObjectType));
		check(LObjectInitializer->Object == NewObject);

		// trigger constructor
		// @todo : need to support arbitrary number of constructor parameters forwarding
		CommonTypeHelper->TriggerConstructor(NewObject);

		// set the class to NewObject
		NewObject->Class = LObjectInitializer->Class;

		// generate unique object name
		NewObject->GenerateName();

		// reset the ObjectInitializer
		LObjectInitializer->Reset();

		// register new object to GObjectArray
		unique_ptr<HObject> NewObjectPtr(NewObject);
		HObjectArrayData NewData = HObjectArray::GetSingleton()->RegisterObject(NewObjectPtr, 0);

		if (bNeedRootSet)
		{
			// if we need to set root set
			HObjectHelper::SetAsRootSet(NewData);
		}

		return NewData;
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

IMPLEMENT_CLASS_TYPE(HCoreObject)
IMPLEMENT_CLASS_TYPE1(HObject, HCoreObject)
