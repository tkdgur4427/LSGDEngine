#include "HEnginePCH.h"
#include "HIpDriver.h"

#include "HIpDriverImpl.h"

using namespace lsgd;

IMPLEMENT_CLASS_TYPE1(HIpDriver, HObject)

void HIpDriver::Reflect()
{

}

// HIpDriver

void HIpDriver::Initialize()
{
	Implementation = make_shared<HIpDriverImpl, HTcpIpDriverImpl>(*this);
	Implementation->Init();
}

void HIpDriver::Destroy()
{
	if (Implementation)
	{
		Implementation->Destroy();
	}
}

void HIpDriver::Tick(float DeltaTime)
{
	check(Implementation);

	// process pending connections
	ProcessPendingConnections();

	// tick client connections
	for (auto& ClientConnection : ClientConnections)
	{
		ClientConnection->Tick(DeltaTime);
	}
}

void HIpDriver::ProcessPendingConnections()
{
	HArray<HConnectionCreationDescription> CurrPendingConnections;
	PendingConnections.PopAll(CurrPendingConnections);

	for (auto& Connection : CurrPendingConnections)
	{
		// add new connection to HIpDriver

		// create new client connection
		HObjectHandleUnique<HNetConnection> NewConnection = HObjectHandleUnique<HNetConnection>(AllocateHObject(HNetConnection::GetClassName()));

		// set socket name
		NewConnection->SocketDesc = Connection.SocketDesc;

		// add client connection
		ClientConnections.push_back(HMove(NewConnection));
	}
}

void HIpDriver::HandleListenerConnectionAccepted(const HString& SocketDesc)
{
	// @todo - currently it is not supporting MT HObject creation
	HConnectionCreationDescription NewConnectionCreationDesc;
	NewConnectionCreationDesc.SocketDesc = SocketDesc;

	PendingConnections.Push(NewConnectionCreationDesc);
}

void HIpDriver::DisconnectClient(const HString& SocketDesc)
{
	Implementation->DisconnectSocket(SocketDesc);
}

void HIpDriver::HandleRecvCommand(const HString& InSocketDesc, uint8* InBuffer, uint32 InSize)
{
	// find the connection who has same socket name
	HNetConnection* NetConnection = nullptr;
	for (auto& ClientConnection : ClientConnections)
	{
		if (ClientConnection->SocketDesc == InSocketDesc)
		{
			NetConnection = ClientConnection.Get();
		}
	}
	check(NetConnection != nullptr);

	// process the buffer with
}
