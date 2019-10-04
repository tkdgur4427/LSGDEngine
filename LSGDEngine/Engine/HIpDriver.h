#pragma once

#include "HNetConnection.h"

namespace lsgd {

	// forward declaration
	namespace networking
	{
		class ISocketSubsystem;
	}

	// represents a packet received and/or error encountered by the receive thread, if enabled, queued for the game thread to process
	struct HReceivePacket
	{
		// content of the packet as received from the socket
		HArray<uint8> PacketBytes;
		// socket description
		HString SocketDesc;
		// the error triggered by the socket RecvFrom call
		HString Error;
		// FPlatfromTime::Seconds() at which this packet and/or error was received; can be used for more accurate ping calculation
		double PlatformTimeSeconds;
	};

	// represents a packet to send
	struct HSendPacket
	{
		HString SocketDesc;
		HArray<uint8> PacketBytes;
	};

	DECLARE_CLASS_TYPE1(HIpDriver, HObject)
	class HIpDriver : public HObject
	{
	public:
		GENERATE_CLASS_BODY(HIpDriver)

		HIpDriver() {}
		virtual ~HIpDriver() {}

		virtual void Initialize(bool InbRunAsServer = true);
		virtual void Destroy();
		virtual void Tick(float DeltaTime);

		// send 
		void SendPacket(HNetConnection* InNetConnection, const HArray<uint8>& PacketBytesToSend);

		// tick
		void ProcessPendingConnections();
		void ProcessPendingReceivePackets();
		void ProcessPendingSendPackets();

		// communication line to HIpDriverImpl
		void HandleListenerConnectionAccepted(const HString& SocketDesc);
		void DisconnectClient(const HString& SocketDesc);
		bool HandleRecvCommand(const HReceivePacket& InPacket);

		// deferred connection creation
		struct HConnectionCreationDescription
		{
			HString SocketDesc;
		};

		struct HPendingConnectionStateToReceive
		{
			HNetConnection* ClientConnection;
			HArray<HReceivePacket> ReceivePackets;
		};

		struct HPendingConnectionStateToSend
		{
			HNetConnection* ClientConnection;
			HArray<HSendPacket> SendPackets;
		};

		// @todo - need to wrap this up
		// whether this IpDriver is used as server or not
		bool bRunAsServer;
		HString ServerAddrStr;
		int32 ServerPort;

		// implementation
		shared_ptr<class HIpDriverImpl> Implementation;

		HObjectHandleUnique<HNetConnection> ServerConnection;
		HArray<HObjectHandleUnique<HNetConnection>> ClientConnections;

		// pending connection to create HNetConnection
		container::HConcurrentQueue<HConnectionCreationDescription> PendingConnections;

		// pending connection state to apply received/send packets
		// @todo - need to think that to integrate this pending state to NetConnection class
		HArray<HPendingConnectionStateToReceive> PendingConnectionStatesToReceive;
		HArray<HPendingConnectionStateToSend> PendingConnectionStatesToSend;
	};
}