#include "HEnginePCH.h"
#include "HSocketBSD.h"

using namespace lsgd;
using namespace lsgd::networking;

HSocketSubsystemWindows* HSocketSubsystemWindows::SocketSingleton = nullptr;

int32 HInternetAddrBSD::GetStorageSize() const
{
	// @todo - only supporting IPv4 only currently...
	return sizeof(sockaddr_in);
}

void HInternetAddrBSD::SetIp(uint32 InAddr)
{
	((sockaddr_in*)& Addr)->sin_addr.s_addr = htonl(InAddr);
	Addr.ss_family = AF_INET;
}

void HInternetAddrBSD::SetPort(int32 InPort)
{
	((sockaddr_in*)& Addr)->sin_port = htons(InPort);
}

shared_ptr<HInternetAddrBSD> HInternetAddrBSD::CreateInternetAddr(uint32 Address, uint32 Port)
{
	shared_ptr<HInternetAddrBSD> NewAddr = make_shared<HInternetAddrBSD>();
	if (Address == 0) // when address is 0, it means that SetAnyIpv4Address
	{
		NewAddr->SetAnyIPv4Address();
	}
	else
	{
		NewAddr->SetIp(Address);
	}
	NewAddr->SetPort(Port);
	return NewAddr;
}

void HInternetAddrBSD::GetIp(uint32& OutAddr) const
{
	OutAddr = ntohl(((sockaddr_in*)& Addr)->sin_addr.s_addr);
}

int32 HInternetAddrBSD::GetPort() const
{
	return ntohs(((sockaddr_in*)& Addr)->sin_port);
}

void HInternetAddrBSD::SetAnyIPv4Address()
{
	Clear();
	((sockaddr_in*)& Addr)->sin_addr.s_addr = htonl(INADDR_ANY);
	Addr.ss_family = AF_INET;
}

HString HInternetAddrBSD::ToString(bool bAppendPort) const
{
	HString ReturnVal;
	char IPStr[NI_MAXHOST];
	if (getnameinfo((const sockaddr*)& Addr, GetStorageSize(), IPStr, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST) == 0)
	{
		ReturnVal = IPStr;

		if (bAppendPort)
		{
			ReturnVal += HStringPrintf(":%d", GetPort());
		}
	}
	return ReturnVal;
}

void HInternetAddrBSD::Clear()
{
	HGenericMemory::MemZero(&Addr, 0, sizeof(Addr));
	Addr.ss_family = AF_UNSPEC;
}

HSocketBSD::HSocketBSD(SOCKET InSocket, ESocketType InSocketType, const HString& InSocketDesc, ESocketProtocolFamily InSocketProtocol, ISocketSubsystem* InSocketSubsystem)
	: SocketType(InSocketType)
	, SocketDescription(InSocketDesc)
	, SocketProtocol(InSocketProtocol)
	, Socket(InSocket)
	, LastActivityTime(0.0)
	, SocketSubsystem(InSocketSubsystem)
{}

bool HSocketBSD::Shutdown(ESocketShutdownMode Mode)
{
	int32 InternalMode = 0;

#if SGD_PLATFORM_WINDOWS
	switch (Mode)
	{
	case ESocketShutdownMode::SSM_Read:
		InternalMode = SD_RECEIVE;
		break;
	case ESocketShutdownMode::SSM_Write:
		InternalMode = SD_SEND;
		break;
	case ESocketShutdownMode::SSM_ReadWrite:
		InternalMode = SD_BOTH;
		break;
	}
#endif

	return shutdown(Socket, InternalMode) == 0;
}

bool HSocketBSD::Close()
{
	if (Socket != INVALID_SOCKET)
	{
		int32 Error = closesocket(Socket);
		Socket = INVALID_SOCKET;
		return Error == 0;
	}
	return false;
}

bool HSocketBSD::Bind(const HInternetAddrBSD& Addr)
{
	return bind(Socket, (const sockaddr*)&(Addr.Addr), Addr.GetStorageSize()) >= 0;
}

bool HSocketBSD::Connect(const HInternetAddrBSD& Addr)
{
	int32 Return = connect(Socket, (const sockaddr*)& Addr.Addr, Addr.GetStorageSize());
	check(SocketSubsystem);

	ESocketErrors Error = SocketSubsystem->TranslateErrorCode(Return);

	// EWOULDBLOCK is not an error, and EINPROGRESS is fine on initial connection as it may still be creating for nonblocking sockets
	return ((Error == SE_NO_ERROR) || Error == SE_EWOULDBLOCK || (Error == SE_EINPROGRESS));
}

bool HSocketBSD::Listen(int32 MaxBacklog)
{
	int32 Result = listen(Socket, MaxBacklog);
	if (Result < 0)
	{
		// something wrong happen!
		ESocketErrors Error = SocketSubsystem->TranslateErrorCode(Result);
		check(Error == ESocketErrors::SE_NO_ERROR);
	}	
	return Result >= 0;
}

HSocketBSD* HSocketBSD::Accept(const HString& InSocketDescription)
{
	SOCKET NewSocket = accept(Socket, nullptr, nullptr);

	if (NewSocket != INVALID_SOCKET)
	{
		// we need the subclass to create the actual FSocket object
		check(SocketSubsystem);
		HSocketSubsystemBSD* BSDSystem = static_cast<HSocketSubsystemBSD*>(SocketSubsystem);
		return BSDSystem->InternalBSDSocketFactory(NewSocket, SocketType, InSocketDescription, SocketProtocol);
	}

	return nullptr;
}

HSocketBSD* HSocketBSD::Accept(HInternetAddrBSD& OutAddr, const HString& InSocketDescription)
{
	int32 SizeOf = sizeof(sockaddr_storage);
	SOCKET NewSocket = accept(Socket, (sockaddr*)&(OutAddr.Addr), &SizeOf);

	if (NewSocket != INVALID_SOCKET)
	{
		check(SocketSubsystem);
		HSocketSubsystemBSD* BSDSystem = static_cast<HSocketSubsystemBSD*>(SocketSubsystem);
		return BSDSystem->InternalBSDSocketFactory(NewSocket, SocketType, InSocketDescription, SocketProtocol);
	}
	
	return nullptr;
}

void HSocketBSD::GetAddress(HInternetAddrBSD& OutAddr)
{
	int32 Size = sizeof(sockaddr_storage);

	// figure out what ip/port we are bound to
	bool bOk = getsockname(Socket, (sockaddr*)&OutAddr.Addr, &Size) == 0;
	check(bOk);
}

bool HSocketBSD::SetNonBlocking(bool bIsNonBlocking)
{
	u_long Value = bIsNonBlocking ? true : false;
	return ioctlsocket(Socket, FIONBIO, &Value) >= 0;
}

bool HSocketBSD::SetNoDelay(bool bIsNoDelay)
{
	// only valid when tcp socket!
	check(SocketType == ST_Streaming);

	int32 Value = bIsNoDelay ? true : false;
	return setsockopt(Socket, IPPROTO_TCP, TCP_NODELAY, (char*)&Value, sizeof(int32)) == 0;
}

ESocketBSDReturn HSocketBSD::HasState(ESocketBSDParam State)
{
	// convert wait-time to time-val
	timeval Time;
	Time.tv_sec = 0;
	Time.tv_usec = 0;

	fd_set SocketSet;

	// -- set up the socket sets we are interested in

	// empty the set
	FD_ZERO(&SocketSet);
	// insert the fd_set to the socket
	FD_SET(Socket, &SocketSet);

	timeval* TimePointer = &Time;

	// check the status of the state
	int32 SelectStatus = 0;
	switch (State)
	{
	case ESocketBSDParam::SP_CanRead:
		SelectStatus = select(Socket + 1, &SocketSet, nullptr, nullptr, TimePointer);
		break;
	case ESocketBSDParam::SP_CanWrite:
		SelectStatus = select(Socket + 1, nullptr, &SocketSet, nullptr, TimePointer);
		break;
	case ESocketBSDParam::SP_HasError:
		SelectStatus = select(Socket + 1, nullptr, nullptr, &SocketSet, TimePointer);
		break;
	}

	// if the select returns a positive number, the socket has the state, 0 means didn't have it, and negative is API error condition
	return SelectStatus > 0 ? ESocketBSDReturn::SR_Yes : SelectStatus == 0 ? ESocketBSDReturn::SR_No : ESocketBSDReturn::SR_EncounteredError;
}

bool HSocketBSD::WaitForPendingConnection(bool& bHasPendingConnection)
{
	bool bHasSucceeded = false;
	bHasPendingConnection = false;

	// make sure socket has no error state
	if (HasState(ESocketBSDParam::SP_HasError) == ESocketBSDReturn::SR_No)
	{
		// get the read state
		ESocketBSDReturn State = HasState(ESocketBSDParam::SP_CanRead);
		// turn the result into the outputs
		bHasSucceeded = State != ESocketBSDReturn::SR_EncounteredError;
		bHasPendingConnection = State == ESocketBSDReturn::SR_Yes;
	}

	return bHasSucceeded;
}

bool HSocketBSD::HasPendingData(uint32& PendingDataSize)
{
	PendingDataSize = 0;

	// make sure socket has no error state
	if (HasState(ESocketBSDParam::SP_CanRead) == ESocketBSDReturn::SR_Yes)
	{
		// see if there is any pending data on the read socket
		if (ioctlsocket(Socket, FIONREAD, (u_long*)& PendingDataSize) == 0)
		{
			return (PendingDataSize > 0);
		}
	}
	return false;
}

bool HSocketBSD::Recv(uint8* Data, int32 BufferSize, int32& BytesRead)
{
	bool bSuccess = false;
	const bool bStreamSocket = (SocketType == ESocketType::ST_Streaming);
	BytesRead = recv(Socket, (char*)Data, BufferSize, 0);

	if (BytesRead >= 0)
	{
		// for streaming sockets, 0 indicates a graceful failure
		bSuccess = !bStreamSocket || (BytesRead > 0);
	}
	else
	{
		// for streaming socket, don't treat SE_EWOULDBLOCK as an error
		bSuccess = bStreamSocket && (SocketSubsystem->TranslateErrorCode(BytesRead) == SE_EWOULDBLOCK);
		BytesRead = 0;
	}

	if (bSuccess)
	{
		LastActivityTime = HGenericPlatformMisc::GetSeconds();
	}

	return bSuccess;
}

bool HSocketBSD::Send(const uint8* Data, int32 Count, int32& BytesSend)
{
	BytesSend = send(Socket, (const char*)Data, Count, 0);

	bool Result = BytesSend >= 0;
	if (Result)
	{
		LastActivityTime = HGenericPlatformMisc::GetSeconds();
	}
	return Result;
}

bool HSocketBSD::Wait(ESocketWaitConditions::Type Condition)
{
	check(false); // no blocking supported...
	return true;
}

ESocketConnectionState HSocketBSD::GetConnectionState()
{
	ESocketConnectionState CurrentState = ESocketConnectionState::SCS_ConnectionError;

	// look for an existing error
	if (HasState(ESocketBSDParam::SP_HasError) == ESocketBSDReturn::SR_No)
	{
		if (HGenericPlatformMisc::GetSeconds() - LastActivityTime > 5.0)
		{
			// get the write state
			ESocketBSDReturn WriteState = HasState(ESocketBSDParam::SP_CanWrite);
			ESocketBSDReturn ReadState = HasState(ESocketBSDParam::SP_CanRead);

			// translate yes or no (error is already set)
			if (WriteState == ESocketBSDReturn::SR_Yes || ReadState == ESocketBSDReturn::SR_Yes)
			{
				CurrentState = ESocketConnectionState::SCS_Connected;
				LastActivityTime = HGenericPlatformMisc::GetSeconds();
			}
			else if (WriteState == ESocketBSDReturn::SR_No && ReadState == ESocketBSDReturn::SR_No)
			{
				CurrentState = ESocketConnectionState::SCS_NotConnected;
			}
		}
		else
		{
			CurrentState = ESocketConnectionState::SCS_Connected;
		}
	}

	return CurrentState;
}

bool HSocketBSD::SetLinger(bool bShouldLinger, int32 Timeout)
{
	linger ling;
	ling.l_onoff = bShouldLinger;
	ling.l_linger = Timeout;

	return setsockopt(Socket, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling)) >= 0;
}

int32 HSocketBSD::GetPortNo()
{
	sockaddr_storage Addr;
	int32 Size = sizeof(sockaddr_storage);

	// figure out what ip/port we are bound to
	bool bOk = getsockname(Socket, (sockaddr*)& Addr, &Size) == 0;
	if (bOk == false)
	{
		return 0;
	}
	return ntohs(((sockaddr_in&)Addr).sin_port);
}

bool HSocketBSD::SetReuseAddr(bool bAllowReuse)
{
	int32 Param = bAllowReuse ? 1 : 0;
	int32 ReuseAddrResult = setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, (char*)&Param, sizeof(Param));
#if SO_REUSEPORT
	if (ReuseAddrResult)
	{
		return setsockopt(Socket, SOL_SOCKET, SO_REUSEPORT, (char*)& Param, sizeof(Param)) == 0;
	}
#endif
	return ReuseAddrResult >= 0;
}

bool HSocketBSD::SetSendBufferSize(int32 Size, int32& NewSize)
{
	int32 SizeSize = sizeof(int32);
	bool bOk = setsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& Size, SizeSize) == 0;

	// read back the value we set
	getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& NewSize, &SizeSize) == 0;

	return bOk;
}

bool HSocketBSD::SetRecvBufferSize(int32 Size, int32& NewSize)
{
	int32 SizeSize = sizeof(int32);
	bool bOk = setsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& Size, SizeSize) == 0;

	// read back the value we set
	getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& NewSize, &SizeSize) == 0;

	return bOk;
}

int32 HSocketSubsystemBSD::GetProtocolFamilyValue(ESocketProtocolFamily InProtocol) const
{
	switch (InProtocol)
	{
	default:
	case ESocketProtocolFamily::SPF_IPv4:
		return AF_INET;
	}
}

HSocketBSD* HSocketSubsystemBSD::CreateSocket(const HString& InSocketType, const HString& InSocketDescription, ESocketProtocolFamily ProtocolType)
{
	SOCKET Socket = INVALID_SOCKET;
	HSocketBSD* NewSocket = nullptr;
	int32 PlatformSpecificTypeFlags = 0;

	/*
		for platforms that have two subsystems (ex: Stream) but don't explicitly inherit from SocketSubsystemBSD
		so they don't know which protocol to end up using and pass in None
		this is invalid, so we need to attempt to still resolve it
	*/
	//...

	if (ProtocolType != ESocketProtocolFamily::SPF_IPv4)
	{
		return nullptr;
	}

	bool bIsUDP = (InSocketType == "DGram") ? true : false;
	int32 SocketTypeFlag = (bIsUDP) ? SOCK_DGRAM : SOCK_STREAM;

	Socket = socket(GetProtocolFamilyValue(ProtocolType), SocketTypeFlag | PlatformSpecificTypeFlags, ((bIsUDP) ? IPPROTO_UDP : IPPROTO_TCP));
	{
		NewSocket = (Socket != INVALID_SOCKET) ? InternalBSDSocketFactory(Socket, ((bIsUDP) ? ESocketType::ST_Datagram : ESocketType::ST_Streaming), InSocketDescription, ProtocolType) : nullptr;
	}

	if (!NewSocket)
	{
		check(NewSocket == nullptr);
		return nullptr;
	}

	return NewSocket;
}

HSocketBSD* HSocketSubsystemBSD::InternalBSDSocketFactory(SOCKET Socket, ESocketType InSocketType, const HString& InSocketDescription, ESocketProtocolFamily InSocketProtocol)
{
	return new HSocketBSD(Socket, InSocketType, InSocketDescription, InSocketProtocol, this);
}

ESocketErrors HSocketSubsystemBSD::TranslateErrorCode(int32 Code)
{
	check(false); // not implemented!
	// @todo - not implemented
	return ESocketErrors::SE_NO_ERROR;
}

HSocketSubsystemWindows* HSocketSubsystemWindows::Create()
{
	if (SocketSingleton == nullptr)
	{
		SocketSingleton = new HSocketSubsystemWindows();

		bool bResult = SocketSingleton->Init();
		check(bResult);
	}
	return SocketSingleton;
}

void HSocketSubsystemWindows::Destroy()
{
	if (SocketSingleton != nullptr)
	{
		SocketSingleton->Shutdown();
		delete SocketSingleton;
		SocketSingleton = nullptr;
	}
}

HSocketSubsystemWindows* HSocketSubsystemWindows::Get()
{
	if (SocketSingleton)
	{
		return SocketSingleton;
	}
	return nullptr;
}

bool HSocketSubsystemWindows::Init()
{
	bool bSuccess = false;
	if (bTriedToInit == false)
	{
		bTriedToInit = true;
		WSADATA WSAData;

		int32 Code = WSAStartup(0x0101, &WSAData);
		if (Code == 0)
		{
			bSuccess = true;
		}
	}
	check(bSuccess);
	return bSuccess;
}

void HSocketSubsystemWindows::Shutdown()
{
	WSACleanup();
}

HSocketBSD* HSocketSubsystemWindows::CreateSocket(const HString& InSocketType, const HString& InSocketDescription, ESocketProtocolFamily ProtocolType)
{
	HSocketBSD* NewSocket = (HSocketBSD*)HSocketSubsystemBSD::CreateSocket(InSocketType, InSocketDescription, ProtocolType);
	check(NewSocket != nullptr);
	return NewSocket;
}

ESocketErrors HSocketSubsystemWindows::GetLastErrorCode()
{
	return TranslateErrorCode(WSAGetLastError());
}

ESocketErrors HSocketSubsystemWindows::TranslateErrorCode(int32 Code)
{
	// handle the generic -1 error
	if (Code == SOCKET_ERROR)
	{
		return GetLastErrorCode();
	}

	switch (Code)
	{
	case 0: return SE_NO_ERROR;
	case ERROR_INVALID_HANDLE: return SE_ECONNRESET; // invalid socket handle catch
	case WSAEINTR: return SE_EINTR;
	case WSAEBADF: return SE_EBADF;
	case WSAEACCES: return SE_EACCES;
	case WSAEFAULT: return SE_EFAULT;
	case WSAEINVAL: return SE_EINVAL;
	case WSAEMFILE: return SE_EMFILE;
	case WSAEWOULDBLOCK: return SE_EWOULDBLOCK;
	case WSAEINPROGRESS: return SE_EINPROGRESS;
	case WSAEALREADY: return SE_EALREADY;
	case WSAENOTSOCK: return SE_ENOTSOCK;
	case WSAEDESTADDRREQ: return SE_EDESTADDRREQ;
	case WSAEMSGSIZE: return SE_EMSGSIZE;
	case WSAEPROTOTYPE: return SE_EPROTOTYPE;
	case WSAENOPROTOOPT: return SE_ENOPROTOOPT;
	case WSAEPROTONOSUPPORT: return SE_EPROTONOSUPPORT;
	case WSAESOCKTNOSUPPORT: return SE_ESOCKTNOSUPPORT;
	case WSAEOPNOTSUPP: return SE_EOPNOTSUPP;
	case WSAEPFNOSUPPORT: return SE_EPFNOSUPPORT;
	case WSAEAFNOSUPPORT: return SE_EAFNOSUPPORT;
	case WSAEADDRINUSE: return SE_EADDRINUSE;
	case WSAEADDRNOTAVAIL: return SE_EADDRNOTAVAIL;
	case WSAENETDOWN: return SE_ENETDOWN;
	case WSAENETUNREACH: return SE_ENETUNREACH;
	case WSAENETRESET: return SE_ENETRESET;
	case WSAECONNABORTED: return SE_ECONNABORTED;
	case WSAECONNRESET: return SE_ECONNRESET;
	case WSAENOBUFS: return SE_ENOBUFS;
	case WSAEISCONN: return SE_EISCONN;
	case WSAENOTCONN: return SE_ENOTCONN;
	case WSAESHUTDOWN: return SE_ESHUTDOWN;
	case WSAETOOMANYREFS: return SE_ETOOMANYREFS;
	case WSAETIMEDOUT: return SE_ETIMEDOUT;
	case WSAECONNREFUSED: return SE_ECONNREFUSED;
	case WSAELOOP: return SE_ELOOP;
	case WSAENAMETOOLONG: return SE_ENAMETOOLONG;
	case WSAEHOSTDOWN: return SE_EHOSTDOWN;
	case WSAEHOSTUNREACH: return SE_EHOSTUNREACH;
	case WSAENOTEMPTY: return SE_ENOTEMPTY;
	case WSAEPROCLIM: return SE_EPROCLIM;
	case WSAEUSERS: return SE_EUSERS;
	case WSAEDQUOT: return SE_EDQUOT;
	case WSAESTALE: return SE_ESTALE;
	case WSAEREMOTE: return SE_EREMOTE;
	case WSAEDISCON: return SE_EDISCON;
	case WSASYSNOTREADY: return SE_SYSNOTREADY;
	case WSAVERNOTSUPPORTED: return SE_VERNOTSUPPORTED;
	case WSANOTINITIALISED: return SE_NOTINITIALISED;
	case WSAHOST_NOT_FOUND: return SE_HOST_NOT_FOUND;
	case WSATRY_AGAIN: return SE_TRY_AGAIN;
	case WSANO_RECOVERY: return SE_NO_RECOVERY;
	case WSANO_DATA: return SE_NO_DATA;
		// case : return SE_UDP_ERR_PORT_UNREACH; //@TODO Find it's replacement
	}

	return SE_NO_ERROR;
}
