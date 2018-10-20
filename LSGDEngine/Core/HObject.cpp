#include "HCorePCH.h"
#include "HObject.h"

// HClass
#include "HReflect.h"

using namespace lsgd;
using namespace lsgd::reflect;

HCoreObject::HCoreObject()
{

}

void HCoreObject::GenerateName()
{
	check(Class != nullptr);
	
	// generate unique name
	Name = HName(Class->Name.c_str());
}