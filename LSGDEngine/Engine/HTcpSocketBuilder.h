#pragma once

#include "HSocketBSD.h"

namespace lsgd {

	class HTcpSocketBuilder
	{
	public:
		HTcpSocketBuilder(const HString InDesc)
			: Description(InDesc)
			, bBlocking(false)
			, Bound(false)
			, Linger(false)
			, LingerTimeout(0)
			, Listen(false)
			, ListenBacklog(1)
			, ReceiveBufferSize(0)
			, SendBufferSize(0)
			, Reusable(false)
		{

		}

		~HTcpSocketBuilder() {}

		operator networking::HSocketBSD* () const
		{
			return Build();
		}

		networking::HSocketBSD* Build() const;
		HTcpSocketBuilder& Listening(int32 MaxBacklog);
		HTcpSocketBuilder& AsReusable();
		HTcpSocketBuilder& BoundToEndpoint(shared_ptr<networking::HInternetAddrBSD> InBoundEndpoint);
		HTcpSocketBuilder& WithSendBufferSize(int32 InSendBufferSize);
		HTcpSocketBuilder& WithReceiveBufferSize(int32 InReceiveBufferSize);

		// holds a flag indicating whether socket operations are blocking
		bool bBlocking;
		// holds a flag indicating whether the socket should be bound
		bool Bound;
		// holds the IP address (and port) that the socket will be bound to
		shared_ptr<networking::HInternetAddrBSD> BoundEndpoint;
		// holds the socket's debug description text
		HString Description;
		// holds a flag indicating whether the socket should linger after closing
		bool Linger;
		// holds the amount of time the socket will linger before closing
		int32 LingerTimeout;
		// holds a flag indicating whether the socket should listen for incoming connections
		bool Listen;
		// holds the number of connections to queue up before refusing them
		int32 ListenBacklog;
		// the desired size of the receive buffer in bytes (0 = default)
		int32 ReceiveBufferSize;
		// holds a flag indicating whether the bound address can be reused other sockets
		bool Reusable;
		// the desired size of the send buffer in bytes (0 = default)
		int32 SendBufferSize;
	};

}