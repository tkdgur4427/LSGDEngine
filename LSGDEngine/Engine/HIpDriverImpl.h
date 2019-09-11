#pragma once

#include "HIpDriver.h"

class HTcpListener;
class HTcpReceiver;
class HTcpSender;

namespace lsgd { namespace networking {

	class HSocketBSD;

} }

namespace lsgd
{
	// for clear separation for network implementation
	class HIpDriverImpl
	{
	public:
		HIpDriverImpl(class HIpDriver& InOwner);
		virtual ~HIpDriverImpl() {}

		virtual void Init() = 0;
		virtual void Destroy() = 0;
		virtual void Update() = 0;

		virtual bool HandleListenerConnectionAccepted(networking::HSocketBSD* InSocket) = 0;
		virtual bool DisconnectSocket(const HString& SocketDesc) = 0;
		virtual void HandlePendingSendPackets(const HArray<HSendPacket>& SendPacketsToHandle) = 0;
		virtual void BatchingPendingSendQueue(HArray<HSendPacket>& OutPendingSendQueue) = 0;

		class HIpDriver& Owner;
	};

	class HTcpIpDriverImpl : public HIpDriverImpl
	{
	public:
		HTcpIpDriverImpl(class HIpDriver& InOwner)
			: HIpDriverImpl(InOwner)
		{}

		virtual ~HTcpIpDriverImpl();

		virtual void Init() override;
		virtual void Destroy() override;
		virtual void Update() override;

		virtual bool HandleListenerConnectionAccepted(networking::HSocketBSD* InSocket) override;
		virtual bool DisconnectSocket(const HString& SocketDesc) override;
		virtual void HandlePendingSendPackets(const HArray<HSendPacket>& SendPacketsToHandle) override;
		virtual void BatchingPendingSendQueue(HArray<HSendPacket>& OutPendingSendQueue) override;

		// global locker for HTcpIpDriverImpl
		HCriticalSection TcpIpDriverImplCS;

		// socket listeners
		shared_ptr<HTcpListener> TcpListener;

		// receiver thread
		shared_ptr<HTcpReceiver> TcpReceiver;

		// sender thread
		shared_ptr<HTcpSender> TcpSender;

		// connected socket states
		struct HConnectedSocketState
		{
			HString SocketDesc;
			networking::HSocketBSD* Socket;
			HArray<uint8> ReceivedBuffer;
			HArray<uint8> SendBuffer;
		};

		HHashMap<HString, HConnectedSocketState> ConnectedSockets;

		// received queue
		HArray<HReceivePacket> ReceiveQueue;
		// send queue
		HArray<HSendPacket> SendQueue;
	};
}
