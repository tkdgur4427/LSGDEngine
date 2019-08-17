#pragma once

namespace lsgd { namespace networking {

	// indicates the type of socket being used (streaming or datagram)
	enum ESocketType
	{
		ST_Unknown,
		ST_Datagram,	// UDP
		ST_Streaming,	// TCP
	};

	// the socket protocol of socket being used, typically for BSD sockets
	enum class ESocketProtocolFamily : uint8
	{
		SPF_None,
		SPF_IPv4,
		SPF_IPv6,
	};

	// enumerate socket shutdown modes
	enum class ESocketShutdownMode
	{
		SSM_Read,
		SSM_Write,
		SSM_ReadWrite,
	};

	enum class ESocketBSDReturn
	{
		SR_Yes,
		SR_No,
		SR_EncounteredError,
	};

	enum class ESocketBSDParam
	{
		SP_CanRead,
		SP_CanWrite,
		SP_HasError,
	};

	enum class ESocketConnectionState
	{
		SCS_Connected,
		SCS_NotConnected,
		SCS_ConnectionError,
	};

	namespace ESocketWaitConditions
	{
		enum Type
		{
			WaitForRead,
			WaitForWrite,
			WaitForReadOrWrite,
		};
	}

	// all supported error types by the engine, mapped from platform specific values
	enum ESocketErrors
	{
		SE_NO_ERROR,
		SE_EINTR,
		SE_EBADF,
		SE_EACCES,
		SE_EFAULT,
		SE_EINVAL,
		SE_EMFILE,
		SE_EWOULDBLOCK,
		SE_EINPROGRESS,
		SE_EALREADY,
		SE_ENOTSOCK,
		SE_EDESTADDRREQ,
		SE_EMSGSIZE,
		SE_EPROTOTYPE,
		SE_ENOPROTOOPT,
		SE_EPROTONOSUPPORT,
		SE_ESOCKTNOSUPPORT,
		SE_EOPNOTSUPP,
		SE_EPFNOSUPPORT,
		SE_EAFNOSUPPORT,
		SE_EADDRINUSE,
		SE_EADDRNOTAVAIL,
		SE_ENETDOWN,
		SE_ENETUNREACH,
		SE_ENETRESET,
		SE_ECONNABORTED,
		SE_ECONNRESET,
		SE_ENOBUFS,
		SE_EISCONN,
		SE_ENOTCONN,
		SE_ESHUTDOWN,
		SE_ETOOMANYREFS,
		SE_ETIMEDOUT,
		SE_ECONNREFUSED,
		SE_ELOOP,
		SE_ENAMETOOLONG,
		SE_EHOSTDOWN,
		SE_EHOSTUNREACH,
		SE_ENOTEMPTY,
		SE_EPROCLIM,
		SE_EUSERS,
		SE_EDQUOT,
		SE_ESTALE,
		SE_EREMOTE,
		SE_EDISCON,
		SE_SYSNOTREADY,
		SE_VERNOTSUPPORTED,
		SE_NOTINITIALISED,
		SE_HOST_NOT_FOUND,
		SE_TRY_AGAIN,
		SE_NO_RECOVERY,
		SE_NO_DATA,
		SE_UDP_ERR_PORT_UNREACH,
		SE_ADDRFAMILY,
		SE_SYSTEM,
		SE_NODEV,

		// this is a special error which means to lookup the most recent error (via GetLastErrorCode())
		SE_GET_LAST_ERROR_CODE,
	};

	class HInternetAddrBSD
	{
	public:
		HInternetAddrBSD() { Clear(); }
		~HInternetAddrBSD() {}

		void Clear();

		int32 GetStorageSize() const;
		void SetIp(uint32 InAddr);
		void SetPort(int32 InPort);
		void GetIp(uint32& OutAddr) const;
		int32 GetPort() const;
		void SetAnyIPv4Address();
		HString ToString(bool bAppendPort) const;

		static shared_ptr<HInternetAddrBSD> CreateInternetAddr(uint32 Address = 0, uint32 Port = 0);

		// the internet ip address structure
		sockaddr_storage Addr;
	};

	class HSocketBSD
	{
	public:
		HSocketBSD(SOCKET InSocket, ESocketType InSocketType, const HString& InSocketDesc, ESocketProtocolFamily InSocketProtocol, class ISocketSubsystem* InSocketSubsystem);
		~HSocketBSD() { Close(); }

		bool Shutdown(ESocketShutdownMode Mode);
		bool Close();
		bool Bind(const HInternetAddrBSD& Addr);
		bool Connect(const HInternetAddrBSD& Addr);
		bool Listen(int32 MaxBacklog);

		HSocketBSD* Accept(const HString& InSocketDescription);
		HSocketBSD* Accept(HInternetAddrBSD& OutAddr, const HString& InSocketDescription);
		
		bool SetNonBlocking(bool bIsNonBlocking);
		bool SetNoDelay(bool bIsNoDelay);

		ESocketBSDReturn HasState(ESocketBSDParam State);
		bool WaitForPendingConnection(bool& bHasPendingConnection);
		bool HasPendingData(uint32& PendingDataSize);
		
		bool Recv(uint8* Data, int32 BufferSize, int32& BytesRead);
		bool Send(const uint8* Data, int32 Count, int32& BytesSend);
		bool Wait(ESocketWaitConditions::Type Condition);
		ESocketConnectionState GetConnectionState();

		void GetAddress(HInternetAddrBSD& OutAddr);
		bool SetLinger(bool bShouldLinger, int32 Timeout);
		int32 GetPortNo();

		bool SetReuseAddr(bool bAllowReuse);
		bool SetSendBufferSize(int32 Size, int32& NewSize);
		bool SetRecvBufferSize(int32 Size, int32& NewSize);

		HString GetDesc() const { return SocketDescription; }

	public:
		// the type of socket
		const ESocketType SocketType;
		// debug desc of socket usage
		HString SocketDescription;
		// protocol used in creation of a socket
		ESocketProtocolFamily SocketProtocol;

		// holds the BSD socket object
		SOCKET Socket;
		// last activity time
		double LastActivityTime;
		// pointer to the subsystem that created it
		class ISocketSubsystem* SocketSubsystem;
	};

	class ISocketSubsystem 
	{
	public:
		virtual int32 GetProtocolFamilyValue(ESocketProtocolFamily InProtocol) const = 0;
		virtual HSocketBSD* CreateSocket(const HString& InSocketType, const HString& InSocketDescription, ESocketProtocolFamily ProtocolType) = 0;
		virtual void DestroySocket(HSocketBSD* InSocket)
		{
			delete InSocket;
		}
		virtual HSocketBSD* InternalBSDSocketFactory(SOCKET Socket, ESocketType InSocketType, const HString& InSocketDescription, ESocketProtocolFamily InSocketProtocol) = 0;
		virtual ESocketErrors TranslateErrorCode(int32 Code) = 0;
	};

	// standard bsd specific socket system implementation
	class HSocketSubsystemBSD : public ISocketSubsystem
	{
	public:
		virtual int32 GetProtocolFamilyValue(ESocketProtocolFamily InProtocol) const override;
		virtual HSocketBSD* CreateSocket(const HString& InSocketType, const HString& InSocketDescription, ESocketProtocolFamily ProtocolType) override;
		virtual HSocketBSD* InternalBSDSocketFactory(SOCKET Socket, ESocketType InSocketType, const HString& InSocketDescription, ESocketProtocolFamily InSocketProtocol) override;
		virtual ESocketErrors TranslateErrorCode(int32 Code) override;
	};

	// standard windows socket system implementation
	class HSocketSubsystemWindows : public HSocketSubsystemBSD
	{
	public:
		HSocketSubsystemWindows()
			: bTriedToInit(false)
		{}

		static HSocketSubsystemWindows* Create();
		static HSocketSubsystemWindows* Get();
		static void Destroy();

		bool Init();
		void Shutdown();

		virtual HSocketBSD* CreateSocket(const HString& InSocketType, const HString& InSocketDescription, ESocketProtocolFamily ProtocolType) override;
		virtual ESocketErrors TranslateErrorCode(int32 Code) override;
		ESocketErrors GetLastErrorCode();

		bool bTriedToInit;
		// holds the single instantiation of this subsystem
		static HSocketSubsystemWindows* SocketSingleton;
	};
} }