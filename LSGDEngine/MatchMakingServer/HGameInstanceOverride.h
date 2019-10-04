#pragma once

#include "..\Engine\HGameInstance.h"

// tcp ip driver
#include "..\Engine\HIpDriver.h"

namespace lsgd
{
	// packet dummy
	DECLARE_CLASS_TYPE(HIocpDummy)
	class HIocpDummy : public HObject
	{
	public:
		GENERATE_CLASS_BODY(HIocpDummy)

		// data to echo data
		HArray<uint8> Data;
	};

	DECLARE_CLASS_TYPE1(HGameInstanceOverride, HGameInstance)
	class HGameInstanceOverride : public HGameInstance
	{
	public:
		GENERATE_CLASS_BODY(HGameInstanceOverride)

		HGameInstanceOverride() {}
		virtual ~HGameInstanceOverride() {}

		virtual void Initialize() override;
		virtual void Destroy() override;
		virtual void Tick(float DeltaTime) override;

		HObjectHandleUnique<HIpDriver> IpDriver;
	};
}

