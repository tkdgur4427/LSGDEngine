#pragma once

#include "HNetConnection.h"

namespace lsgd {

	// forward declaration
	namespace networking
	{
		class ISocketSubsystem;
	}

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

		// tick
		void ProcessPendingConnections();

		// communication line to HIpDriverImpl
		void HandleListenerConnectionAccepted(const HString& SocketDesc);
		void DisconnectClient(const HString& SocketDesc);
		void HandleRecvCommand(const HString& InSocketDesc, uint8* InBuffer, uint32 InSize);

		// deferred connection creation
		struct HConnectionCreationDescription
		{
			HString SocketDesc;
		};

		// implementation
		shared_ptr<class HIpDriverImpl> Implementation;

		HObjectHandleUnique<HNetConnection> ServerConnection;
		HArray<HObjectHandleUnique<HNetConnection>> ClientConnections;

		container::HConcurrentQueue<HConnectionCreationDescription> PendingConnections;
	};
}