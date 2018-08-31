#include "HCorePCH.h"
#include "HReflect.h"

using namespace lsgd::reflect;

void HStruct::AddProperty(lsgd::unique_ptr<HProperty>& InProperty)
{
	Properties.push_back(move(InProperty));
}