#pragma once

#include "HNetConnection.h"

namespace lsgd {

	DECLARE_CLASS_TYPE1(HIpDriver, HObject)
	class HIpDriver : public HObject
	{
	public:
		GENERATE_CLASS_BODY(HIpDriver)

		HIpDriver() {}
		virtual ~HIpDriver() {}

		virtual void Initialize();
		virtual void Destroy();
		virtual void Tick(float DeltaTime);

		void HandleListenerConnectionAccepted(const HString& SocketDesc);
		void DisconnectClient(const HString& SocketDesc);

		// implementation
		shared_ptr<class HIpDriverImpl> Implementation;

		HObjectHandleUnique<HNetConnection> ServerConnection;
		HArray<HObjectHandleUnique<HNetConnection>> ClientConnections;
	};

}