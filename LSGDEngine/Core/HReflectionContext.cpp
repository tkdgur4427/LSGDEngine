#include "HCorePCH.h"
#include "HReflectionContext.h"

// HObject
#include "HObject.h"

using namespace lsgd;
using namespace lsgd::reflect;

HSerializeContext& HSerializeContext::operator<<(HName& Value)
{
	// serialize the name 
	Value.Serialize(*this);
	return *this;
}

HReflectionContext& HSerializeContext::operator<<(HObject*& Value)
{
	// serialize the object
	Value->Serialize(*this);
	return *this;
}
		