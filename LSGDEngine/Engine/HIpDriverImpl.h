#pragma once

class HTcpListener;

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

		// socket listeners
		shared_ptr<HTcpListener> TcpListener;

		// connected socket management
		HHashMap<HString, networking::HSocketBSD*> ConnectedSockets;
	};
}
