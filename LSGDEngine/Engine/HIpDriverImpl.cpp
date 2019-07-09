#include "HEnginePCH.h"
#include "HIpDriverImpl.h"
#include "HIpDriver.h"

// socket
#include "HSocketBSD.h"
// tcp socket builder
#include "HTcpSocketBuilder.h"

using namespace lsgd;
using namespace lsgd::networking;

HIpDriverImpl::HIpDriverImpl(HIpDriver& InOwner)
	: Owner(InOwner)
{

}

HTcpIpDriverImpl::~HTcpIpDriverImpl()
{
	Destroy();
}

// Tcp specific implementations
//	- @todo; if we need this functionality, need to separate this file for HTcpListener implementations
#include "..\Core\HThreadManager.h"
using namespace lsgd::thread;

// implements a runnable that listens for incoming TCP connections
class HTcpListener : public HThreadRunnable
{
public:
	HTcpListener(HTcpIpDriverImpl* InOwner)
		: HThreadRunnable("TcpListener")
		, Owner(InOwner)
	{
		// @todo - temporary setting it as 10343
		int32 PortNumber = 10343;

		shared_ptr<HInternetAddrBSD> LocalAddress = HInternetAddrBSD::CreateInternetAddr(0, PortNumber);
		Endpoint = LocalAddress;

		// trigger the runnable thread
		shared_ptr<HThreadRunnable> ThisRunnable = shared_from_this();
		HThreadManager::GetSingleton()->CreateHardwareThread(ThisRunnable);
	}

	virtual ~HTcpListener()
	{

	}

	void Init()
	{
		// create listen socket reader
		if (Socket == nullptr)
		{
			Socket = HTcpSocketBuilder("TcpListenerSocket")
				.AsReusable()
				.BoundToEndpoint(Endpoint)
				.Listening(8) // @todo magic number...?
				.WithSendBufferSize(2 * 1024 * 1024);
		}

		// it should have listen socket!
		check(Socket);
	}

	virtual void Run() override
	{
		// unique numbering for client
		uint32 ClientNumber = 0;

		// create remote address for reusing
		shared_ptr<HInternetAddrBSD> RemoteAddress = HInternetAddrBSD::CreateInternetAddr();

		// initialize
		Init();

		while (!Stopping)
		{
			bool Pending = false;

			// handle incoming connections
			if (Socket->WaitForPendingConnection(Pending))
			{
				if (Pending)
				{
					HString ClientDesc = HStringPrintf("Client_%d", ClientNumber);
					HSocketBSD* ConnectionSocket = Socket->Accept(*RemoteAddress, ClientDesc);

					if (ConnectionSocket != nullptr)
					{
						bool Accepted = false;
						if (Owner)
						{
							Accepted = Owner->HandleListenerConnectionAccepted(ConnectionSocket);
						}

						if (!Accepted)
						{
							ConnectionSocket->Close();
							HSocketSubsystemWindows::Get()->DestroySocket(ConnectionSocket);
						}
					}
				}
			}
			else
			{
				HGenericPlatformMisc::Sleep(0.2); // um... do we make this magic number
			}
		}
	}

	virtual void Terminate() override
	{
		Stopping = true;
	}

	// holds a flag indicating whether the socket should be deleted in the destructor
	bool DeleteSocket;
	// holds the server end point
	shared_ptr<HInternetAddrBSD> Endpoint;
	// holds the server socket
	HSocketBSD* Socket;
	// holds a flag indicating that the thread is stopping
	bool Stopping;

	// owner
	HTcpIpDriverImpl* Owner;
};


void HTcpIpDriverImpl::Init()
{
	// create tcp listener thread
	TcpListener = make_shared<HTcpListener>(this);
}

void HTcpIpDriverImpl::Destroy()
{
	
}

void HTcpIpDriverImpl::Update()
{

}

bool HTcpIpDriverImpl::HandleListenerConnectionAccepted(networking::HSocketBSD* InSocket)
{
	bool IsExisted = (ConnectedSockets.find(InSocket->GetDesc()) != ConnectedSockets.end());
	check(!IsExisted);

	// insert the socket
	ConnectedSockets.insert({ InSocket->GetDesc(), InSocket });

	// notify callback to owner
	Owner.HandleListenerConnectionAccepted(InSocket->GetDesc());

	return true;
}

bool HTcpIpDriverImpl::DisconnectSocket(const HString& SocketDesc)
{
	bool IsExisted = (ConnectedSockets.find(SocketDesc) != ConnectedSockets.end());
	if (IsExisted)
	{
		ConnectedSockets.erase(SocketDesc);
		return true;
	}
	return false;
}