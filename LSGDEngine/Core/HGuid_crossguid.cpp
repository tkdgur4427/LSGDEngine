#include "HCorePCH.h"
#include "HGuid.h"

// cross guid
#include "../../ThirdParty/crossguid/guid.hpp"
#include "../../ThirdParty/crossguid/guid.cpp"

using namespace lsgd;

struct GuidInstance_crossguid : public HGuid::GuidInstance
{
	GuidInstance_crossguid()
		: HGuid::GuidInstance()
	{
		//Value = xg::newGuid();
	}

	string ToString()
	{
		return Value.str();
	}

	xg::Guid Value;
};

HGuid HGuid::CreateGuid()
{
	shared_ptr<GuidInstance> NewValue = make_shared<GuidInstance, GuidInstance_crossguid>();
	return HGuid(NewValue);
}

string HGuid::ToString()
{
	GuidInstance_crossguid* CrossGuid = (GuidInstance_crossguid*)Value.get();
	check(CrossGuid != nullptr);

	return CrossGuid->ToString();
}