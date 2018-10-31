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
{

}

void HObjectInitializer::InitializeProperties()
{
	check(Object != nullptr && Class != nullptr);

	// start to initialize properties
	//@todo - use the CDO and initialize the properties
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

HObject::HObject(HObjectInitializer& InObjectInitializer)
	: HCoreObject()
{
	// setting current initialized object as this object
	InObjectInitializer.Object = this;
}

void HObject::Serialize(reflect::HReflectionContext& InContext)
{
	check(Class != nullptr);
	Class->SerializeProperties(InContext, (uint8*)this);
}

HObject* AllocateHObjectInternal(HObjectInitializer& ObjectInitializer, reflect::HClass* InClass)
{
	uint32 SizeOfHObject = sizeof(HObject);
	uint32 SizeOfHClass = InClass->GetTotalSize();
	ObjectInitializer.TotalSize = SizeOfHClass + SizeOfHObject;	

	// set the HClass in ObjectInitializer
	ObjectInitializer.Class = InClass;

	// note that we didn't trigger any class constructor, just naive allocation happens
	HObject* AllocatedObject = new (HGenericMemory::Allocate(ObjectInitializer.TotalSize))HObject(ObjectInitializer);
	return AllocatedObject;
}
