#pragma once

#include "..\Engine\HGameInstance.h"

// tcp ip driver
#include "..\Engine\HIpDriver.h"

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HPACKET_CS_MATCH_REQ_LOGIN, HObject)
	class HPACKET_CS_MATCH_REQ_LOGIN : public HObject, public HNetworkEventHandler
	{
	public:
		GENERATE_CLASS_BODY(HPACKET_CS_MATCH_REQ_LOGIN)

		static uint16 GetId() { return en_PACKET_TYPE::en_PACKET_CS_MATCH_REQ_LOGIN; }

		HPACKET_CS_MATCH_REQ_LOGIN() {}
		virtual ~HPACKET_CS_MATCH_REQ_LOGIN() {}

		// instance to override the handle packet
		virtual void HandleEvent(class HNetConnection*) override;

		// packet data
		uint16 Type;
		int64 AccountNo;
		char SessionKey[64];
		uint32 Ver_Code;
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

