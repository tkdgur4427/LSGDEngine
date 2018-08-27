#include "HCorePCH.h"
#include "HGuid.h"

// cross guid
#include "../../ThirdParty/crossguid/guid.hpp"

#if _WINDOWS
#define GUID_WINDOWS
#include "../../ThirdParty/crossguid/guid.cpp"
#endif

using namespace lsgd;

struct GuidInstance_crossguid : public HGuid::GuidInstance
{
	GuidInstance_crossguid()
		: HGuid::GuidInstance()
	{
		Value = xg::newGuid();
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

HGuid::HGuid(const HGuid& other)
{
	// for copy constructor, create new instance and copy the instances
	Value = make_shared<GuidInstance, GuidInstance_crossguid>();
	*this = other;
}

string HGuid::ToString() const
{
	GuidInstance_crossguid* CrossGuid = (GuidInstance_crossguid*)Value.get();
	check(CrossGuid != nullptr);

	return CrossGuid->ToString();
}

HGuid::operator string() const
{
	return ToString();
}

const array<uint8, 16>& HGuid::ToArray() const
{
	GuidInstance_crossguid* CrossGuid = (GuidInstance_crossguid*)Value.get();
	return CrossGuid->Value.bytes();
}

void HGuid::Swap(HGuid& other)
{
	GuidInstance_crossguid* CrossGuid = (GuidInstance_crossguid*)Value.get();
	GuidInstance_crossguid* CrossGuidOther = (GuidInstance_crossguid*)other.Value.get();

	CrossGuid->Value.swap(CrossGuidOther->Value);
}

bool HGuid::IsValid() const
{
	// when guid is empty, it is invalid
	if (Value == nullptr)
	{
		return false;
	}

	GuidInstance_crossguid* CrossGuid = (GuidInstance_crossguid*)Value.get();
	return CrossGuid->Value.isValid();
}

// operators
HGuid& HGuid::operator=(const HGuid& other)
{
	GuidInstance_crossguid* CrossGuid = (GuidInstance_crossguid*)Value.get();
	GuidInstance_crossguid* CrossGuidOther = (GuidInstance_crossguid*)other.Value.get();

	CrossGuid->Value = CrossGuidOther->Value;
	return *this;
}

bool HGuid::operator==(const HGuid& other) const
{
	GuidInstance_crossguid* CrossGuid = (GuidInstance_crossguid*)Value.get();
	GuidInstance_crossguid* CrossGuidOther = (GuidInstance_crossguid*)other.Value.get();

	return CrossGuid->Value == CrossGuidOther->Value;
}

bool HGuid::operator!=(const HGuid& other) const
{
	GuidInstance_crossguid* CrossGuid = (GuidInstance_crossguid*)Value.get();
	GuidInstance_crossguid* CrossGuidOther = (GuidInstance_crossguid*)other.Value.get();

	return CrossGuid->Value != CrossGuidOther->Value;
}

namespace std
{
	template <>
	void swap(lsgd::HGuid &lhs, lsgd::HGuid &rhs)
	{
		lhs.Swap(rhs);
	}
}