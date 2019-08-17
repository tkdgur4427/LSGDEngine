#include "HEnginePCH.h"
#include "HTcpSocketBuilder.h"

using namespace lsgd;
using namespace lsgd::networking;

networking::HSocketBSD* HTcpSocketBuilder::Build() const
{
	HSocketBSD* Socket = nullptr;
	ISocketSubsystem* SocketSubsystem = HSocketSubsystemWindows::Create();

	if (SocketSubsystem)
	{
		shared_ptr<HInternetAddrBSD> BoundEndpointAddr = BoundEndpoint;
		Socket = SocketSubsystem->CreateSocket("Stream", Description, ESocketProtocolFamily::SPF_IPv4);

		if (Socket)
		{
			// @todo - need to add reusable option in socket
			bool Error = !Socket->SetLinger(Linger, LingerTimeout)
				|| !Socket->SetReuseAddr(Reusable);

			if (!Error)
			{
				Error = !Socket->SetNonBlocking(!bBlocking);
			}

			if (!Error)
			{
				Error = Bound && !Socket->Bind(*BoundEndpointAddr);
			}

			if (!Error)
			{
				Error = Listen && !Socket->Listen(ListenBacklog);
			}

			if (!Error)
			{
				int32 OutNewSize;
				if (SendBufferSize > 0)
				{
					Socket->SetSendBufferSize(SendBufferSize, OutNewSize);
				}
				if (ReceiveBufferSize > 0)
				{
					Socket->SetRecvBufferSize(ReceiveBufferSize, OutNewSize);
				}
			}

			if (Error)
			{
				SocketSubsystem->DestroySocket(Socket);
				Socket = nullptr;
			}
		}
	}

	return Socket;
}

HTcpSocketBuilder& HTcpSocketBuilder::Listening(int32 MaxBacklog)
{
	Listen = true;
	ListenBacklog = MaxBacklog;
	return *this;
}

HTcpSocketBuilder& HTcpSocketBuilder::AsReusable()
{
	Reusable = true;
	return *this;
}

HTcpSocketBuilder& HTcpSocketBuilder::BoundToEndpoint(shared_ptr<networking::HInternetAddrBSD> InBoundEndpoint)
{
	BoundEndpoint = InBoundEndpoint;
	Bound = true;
	return *this;
}

HTcpSocketBuilder& HTcpSocketBuilder::WithSendBufferSize(int32 InSendBufferSize)
{
	SendBufferSize = InSendBufferSize;
	return *this;
}

HTcpSocketBuilder& HTcpSocketBuilder::WithReceiveBufferSize(int32 InReceiveBufferSize)
{
	ReceiveBufferSize = InReceiveBufferSize;
	return *this;
}