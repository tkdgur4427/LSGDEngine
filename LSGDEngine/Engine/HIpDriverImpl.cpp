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

#define IPDRIVERIMPL_TEST_DEBUG_CODE 0

// implements a runnable that listens for incoming TCP connections
class HTcpListener : public HThreadRunnable
{
public:
	HTcpListener(HTcpIpDriverImpl* InOwner)
		: HThreadRunnable("TcpListener")
		, Owner(InOwner)
		, Stopping(false)
	{
		// @todo - temporary setting it as 6000
		int32 PortNumber = 6000;

		shared_ptr<HInternetAddrBSD> LocalAddress = HInternetAddrBSD::CreateInternetAddr(0, PortNumber);
		Endpoint = LocalAddress;
	}

	virtual ~HTcpListener()
	{

	}

	void Init()
	{
#if IPDRIVERIMPL_TEST_DEBUG_CODE
		ISocketSubsystem* SocketSubsystem = HSocketSubsystemWindows::Create();
		TestCode();
#endif

		// create listen socket reader
		if (Socket == nullptr)
		{
			Socket = HTcpSocketBuilder("TcpListenerSocket")
				.AsReusable()
				.BoundToEndpoint(Endpoint)
				.Listening(8) // @todo magic number...?
				.WithSendBufferSize(2 * 1024 * 1024)
				.WithReceiveBufferSize(2 * 1024 * 1024);
		}

		// it should have listen socket!
		check(Socket);

		// trigger the runnable thread
		shared_ptr<HThreadRunnable> ThisRunnable = shared_from_this();
		HThreadManager::GetSingleton()->CreateHardwareThread(ThisRunnable);
	}

	virtual void Run() override
	{
		// unique numbering for client
		uint32 ClientNumber = 0;

		// create remote address for reusing
		shared_ptr<HInternetAddrBSD> RemoteAddress = HInternetAddrBSD::CreateInternetAddr();

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
							// increase the client number
							ClientNumber++;
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

#if IPDRIVERIMPL_TEST_DEBUG_CODE
	void TestCode()
	{
		int32 i, len, rc, on = 1;
		int32 listen_sd, max_sd, new_sd;
		int32 desc_ready, end_server = 0;
		int32 close_conn;

		char buffer[80];
		struct sockaddr_in addr;
		struct timeval timeout;
		fd_set master_set, working_set;

		// create an AF_INET6 stream socket to receive incoming connections on
		listen_sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		check(listen_sd >= 0);

		// allow socket descriptor to be reusable
		rc = setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, (char*)& on, sizeof(on));
		check(rc >= 0);

		// set socket to be nonblocking
		//	- all of the sockets for the incoming connections will also be nonblocking since they will inherit that state from the listening socket
		rc = ioctlsocket(listen_sd, FIONBIO, (u_long*)& on);
		check(rc >= 0);

		// bind the socket
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		memcpy(&addr.sin_addr, &in4addr_any, sizeof(addr));
		addr.sin_port = htons(6000);
		rc = bind(listen_sd, (struct sockaddr*)&addr, sizeof(addr));
		check(rc >= 0);

		// set the listen back log
		rc = listen(listen_sd, 32);
		check(rc >= 0);

		// initialize the master fd_set
		FD_ZERO(&master_set);
		max_sd = listen_sd;
		FD_SET(listen_sd, &master_set);

		timeout.tv_sec = 0;
		timeout.tv_usec = 0;

		// loop waiting for incoming connects or for incoming data on any of the connected sockets
		do
		{
			// copy the master fd_set over to the working fd_set
			memcpy(&working_set, &master_set, sizeof(master_set));

			// call select() and wait 3 minutes for it to complete
			rc = select(max_sd + 1, &working_set, 0, 0, &timeout);
			check(rc >= 0);

			// one or more descriptors are readable; need to determine which ones they are
			desc_ready = rc;
			for (i = 0; i <= max_sd && desc_ready > 0; ++i)
			{
				// check to see if this descriptor is ready
				if (FD_ISSET(i, &working_set))
				{
					/*
						a descriptor was found that was readable - one less has to be looked for
						this is being done so that we can stop looking at the working set
						once we have found all of the descriptors that were ready
					*/
					desc_ready -= 1;

					// check to see if this is the listening socket
					if (i == listen_sd)
					{
						/*
							accept all incoming connections that are queued up on the listening socket 
							before we loop back and call select again
						*/
						do 
						{
							/*
								accept each incoming connection
								if accept fails with EWOULDBLOCK, then we have accepted all of them
								Any other failure on accept will cause us to end the server
							*/
							new_sd = accept(listen_sd, 0, 0);
							if (new_sd < 0)
							{
								end_server = TRUE;
								break;
							}

							// add the new incoming connection to the master read set
							FD_SET(new_sd, &master_set);
							if (new_sd > max_sd)
								max_sd = new_sd;

							// loop back up and accept another incoming connection

						} while (new_sd != -1);
					}

					// this is not the listening socket, therefore an existing connection must be readable
					else
					{
						close_conn = 0;

						// receive all incoming data on this socket
						// before we loop back and call select again
						do 
						{
							/*
								recv data on this connection until the recv fails with EWOULDBLOCK
								if any other failure occurs, we will close the connection
							*/
							rc = recv(i, buffer, sizeof(buffer), 0);
							if (rc < 0)
							{
								close_conn = 1;
								break;
							}

							// check to see if the connection has been closed by the client
							if (rc == 0)
							{
								close_conn = 1;
								break;
							}

							// data was received
							len = rc;

							// echo the data back to the client
							rc = send(i, buffer, len, 0);
							if (rc < 0)
							{
								close_conn = 1;
								break;
							}
						} while (1);

						/*
							if the close_conn flag was turned on, we need to clean up this active connection
							this clean up process includes removing the descriptor from the master set and determining
							the new maximum descriptor value based on the bits that are still turned on in the master set
						*/
						if (close_conn)
						{
							//close(i);
							FD_CLR(i, &master_set);
							if (i == max_sd)
							{
								while (FD_ISSET(max_sd, &master_set) == FALSE)
									max_sd -= 1;
							}
						}
					}
				}
			}

		} while (end_server == 0);

		// clean up all of the sockets that are open
		for (i = 0; i <= max_sd; ++i)
		{
			//if (FD_ISSET(i, &master_set))
				//close(i);
		}
	}
#endif

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

// the raw packet structure
struct HPacketHeaderLayout
{
	uint16 PacketSize;
};

// runnable object representing the receive thread, if enabled
class HTcpReceiver : public HThreadRunnable
{
public:
	HTcpReceiver(HTcpIpDriverImpl* InOwner)
		: HThreadRunnable("ReceivePacketThread")
		, Owner(InOwner)
		, ReceiveQueue(1024*32) // @todo temporary magic number
	{
		bIsRunning.store(true);
	}

	virtual ~HTcpReceiver()
	{}

	void Init()
	{
		// trigger the runnable thread
		shared_ptr<HThreadRunnable> ThisRunnable = shared_from_this();
		HThreadManager::GetSingleton()->CreateHardwareThread(ThisRunnable);
	}

	virtual void Run() override
	{
		while (bIsRunning.load() == true)
		{
			HArray<HTcpIpDriverImpl::HConnectedSocketState> ConnectedSocketStates;
			{
				// getting connected sockets by locking TcpIpDriverImplCS
				HScopedLock Lock(Owner->TcpIpDriverImplCS);
				
				// @todo - note that it can be dangerous for not considering disconnection synchronization with this!
				for (auto Iter = Owner->ConnectedSockets.begin(); Iter != Owner->ConnectedSockets.end(); ++Iter)
				{
					ConnectedSocketStates.push_back(Iter->second);
				}
			}

			// make it sleep when there are no connected socket states
			if (ConnectedSocketStates.size() == 0)
			{
				//HGenericPlatformMisc::Sleep(0.02);
			}

			// @todo - need to use taskgraph and process them with task threads
			// collect the received raw data from the socket
			for (auto& State : ConnectedSocketStates)
			{
				uint32 PendingDataSize;
				if (State.Socket->HasPendingData(PendingDataSize))
				{
					int32 BytesToRead = 0;
					
					// get old size
					int32 OldSize = State.ReceivedBuffer.size();
					
					// resize properly
					State.ReceivedBuffer.resize(State.ReceivedBuffer.size() + PendingDataSize);

					// get start-address
					uint8* StartAddress = State.ReceivedBuffer.data() + OldSize;

					// recv packet from socket
					if (State.Socket->Recv(StartAddress, PendingDataSize, BytesToRead))
					{
						check(PendingDataSize == BytesToRead); // should be equal! 
					}
					else
					{
						check(false); // something wrong happened, need to check!
					}
				}
			}

			// @todo - need to use taskgraph and process them with task threads
			// process the packet that can be processed
			for (auto& State : ConnectedSocketStates)
			{
				// retrieve the packet
				HPacketHeaderLayout Layout;
				int32 LayoutSize = sizeof(HPacketHeaderLayout);
				
				HMemoryArchive Archive(State.ReceivedBuffer);
				Archive.bIsSaving = false;

				while (Archive.IsEof(LayoutSize))
				{
					// serialize header layout
					Archive.Serialize((void*)& Layout, LayoutSize);

					// start offset for serialize real packet data excluding header
					uint8* SrcStartAddress = State.ReceivedBuffer.data() + sizeof(Layout);

					// generate received packet
					HReceivePacket ReceivedPacket;
					ReceivedPacket.PacketBytes.resize(Layout.PacketSize);
					HGenericMemory::MemCopy(ReceivedPacket.PacketBytes.data(), SrcStartAddress, Layout.PacketSize);
					ReceivedPacket.PlatformTimeSeconds = HGenericPlatformMisc::GetSeconds();
					ReceivedPacket.SocketDesc = State.SocketDesc;

					// add received queue
					ReceiveQueue.Enqueue(ReceivedPacket);
				}
			}
		}
	}

	virtual void Terminate()
	{
		bIsRunning.store(false);
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

	// thread-safe queue of received packets
	// - the Run() function is the producer
	// - the consumer thread will be temporary UIpDriver 
	// @todo - to be multi-threaded
	container::HCircularQueue<HReceivePacket> ReceiveQueue;
	HAtomic<bool> bIsRunning;

	// owner
	HTcpIpDriverImpl* Owner;
	ISocketSubsystem* SocketSubSystem;
};

void HTcpIpDriverImpl::Init()
{
	// create tcp listener thread
	TcpListener = make_shared<HTcpListener>(this);
	TcpListener->Init();

	// create tcp receiver thread
	TcpReceiver = make_shared<HTcpReceiver>(this);
	TcpReceiver->Init();
}

void HTcpIpDriverImpl::Destroy()
{
	
}

void HTcpIpDriverImpl::Update()
{

}

bool HTcpIpDriverImpl::HandleListenerConnectionAccepted(networking::HSocketBSD* InSocket)
{
	HScopedLock Lock(TcpIpDriverImplCS);

	bool IsExisted = (ConnectedSockets.find(InSocket->GetDesc()) != ConnectedSockets.end());
	check(!IsExisted);

	HConnectedSocketState NewState;
	NewState.SocketDesc = InSocket->GetDesc();
	NewState.Socket = InSocket;

	// insert the socket
	ConnectedSockets.insert({ NewState.SocketDesc, NewState });

	// notify callback to owner
	Owner.HandleListenerConnectionAccepted(InSocket->GetDesc());

	return true;
}

bool HTcpIpDriverImpl::DisconnectSocket(const HString& SocketDesc)
{
	HScopedLock Lock(TcpIpDriverImplCS);

	bool IsExisted = (ConnectedSockets.find(SocketDesc) != ConnectedSockets.end());
	if (IsExisted)
	{
		ConnectedSockets.erase(SocketDesc);
		return true;
	}
	return false;
}