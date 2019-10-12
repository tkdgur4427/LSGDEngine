#pragma once

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HNetworkPacket, HObject)
	class HNetworkPacket : public HObject
	{
	public:
		GENERATE_CLASS_BODY(HNetworkPacket)

		// need to override GetId!!
		static uint16 GetId() { return -1; }

		virtual void HandleEvent(class HNetConnection*) {};
	};
}