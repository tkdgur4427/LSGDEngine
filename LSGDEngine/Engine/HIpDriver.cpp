#include "HEnginePCH.h"
#include "HIpDriver.h"

#include "HIpDriverImpl.h"

#include "HHttpClient.h"

// @todo - temp
#include "HEQueuePolicy.h"

using namespace lsgd;

IMPLEMENT_CLASS_TYPE1(HIpDriver, HObject)

uint16 HPacketUtils::SeekAndGetType(HArray<uint8>& InData)
{
	// get the packet type only
	HMemoryArchive Archive(InData);
	Archive.bIsSaving = false;
	uint16 Type;
	Archive << Type;
	return Type;
}

void HIpDriver::Reflect()
{

}

// HIpDriver
void HIpDriver::Initialize(bool InbRunAsServer)
{
	// set the flag
	bRunAsServer = InbRunAsServer;

	// @todo - just temporary...
	if (bRunAsServer)
	{
		ServerAddrStr = "0.0.0.0";
		ServerPort = 6000;
	}

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

	// update implementation
	Implementation->Update();

	// process pending receive packets
	ProcessPendingReceivePackets();

	// process pending send packets
	ProcessPendingSendPackets();
}

void HIpDriver::SendPacket(HNetConnection* InNetConnection, const HArray<uint8>& PacketBytesToSend)
{
	// find the connection
	HPendingConnectionStateToSend* FoundPendingConnectionState = nullptr;
	for (auto& PendingConnectionState : PendingConnectionStatesToSend)
	{
		if (PendingConnectionState.ClientConnection == InNetConnection)
		{
			check(PendingConnectionState.ClientConnection->SocketDesc == InNetConnection->SocketDesc);
			FoundPendingConnectionState = &PendingConnectionState;
		}
	}

	if (FoundPendingConnectionState == nullptr)
	{
		// create new state
		HPendingConnectionStateToSend NewPendingConnectionState;
		NewPendingConnectionState.ClientConnection = InNetConnection;

		// add new pending connection state
		PendingConnectionStatesToSend.push_back(NewPendingConnectionState);
		FoundPendingConnectionState = &PendingConnectionStatesToSend.back();
	}

	// accumulate the packet to send
	HSendPacket NewPacket;
	NewPacket.SocketDesc = FoundPendingConnectionState->ClientConnection->SocketDesc;
	NewPacket.PacketBytes = PacketBytesToSend;
	FoundPendingConnectionState->SendPackets.push_back(NewPacket);
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

void HIpDriver::ProcessPendingReceivePackets()
{
	// @todo - temporary... echo sending!
	for (auto& PendingConnectionState : PendingConnectionStatesToReceive)
	{
		for (auto& ReceivePacket : PendingConnectionState.ReceivePackets)
		{
			// inspect the packet type
			uint16 PacketType = HPacketUtils::SeekAndGetType(ReceivePacket.PacketBytes);

			// create packet
			HObjectHandleUnique<HObject> NewPacket = HPacketRegisterSystem::CreatePacketWithId(PacketType);
			
			// create archive
			HMemoryArchive Archive(ReceivePacket.PacketBytes);
			Archive.bIsSaving = false;

			// serialize the packet
			NewPacket->Serialize(Archive);

			HObject* Temp = NewPacket.Get();
		}

		//shared_ptr<HNetworkEvents> Handler = make_shared<HNetworkEvents, HNetworkEventHandler>();
		//EQueuePolicy<HNetworkEventsBus::ContextPolicy>::QueueEvent(&HNetworkEventHandler::Execute, Handler);
		//EQueuePolicy<HNetworkEventsBus::ContextPolicy>::ExecuteQueuedEvents();

		/*
		// just echo the packet
		HPendingConnectionStateToSend NewConnectionState;
		NewConnectionState.ClientConnection = PendingConnectionState.ClientConnection;
		
		for (auto& ReceivePacket : PendingConnectionState.ReceivePackets)
		{
			HSendPacket NewPacket;
			NewPacket.SocketDesc = ReceivePacket.SocketDesc;
			NewPacket.PacketBytes = ReceivePacket.PacketBytes;

			NewConnectionState.SendPackets.push_back(NewPacket);
		}

		PendingConnectionStatesToSend.push_back(NewConnectionState);
		*/
	}

	// empty pending connection state
	PendingConnectionStatesToReceive.clear();
}

void HIpDriver::ProcessPendingSendPackets()
{
	for (auto& PendingConnectionState : PendingConnectionStatesToSend)
	{
		Implementation->HandlePendingSendPackets(PendingConnectionState.SendPackets);
	}
	
	// empty pending connection states
	PendingConnectionStatesToSend.clear();
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

bool HIpDriver::HandleRecvCommand(const HReceivePacket& InPacket)
{
	// find the connection who has same socket name
	HNetConnection* NetConnection = nullptr;
	for (auto& ClientConnection : ClientConnections)
	{
		if (ClientConnection->SocketDesc == InPacket.SocketDesc)
		{
			NetConnection = ClientConnection.Get();
		}
	}
	
	if (NetConnection == nullptr)
	{
		// still pending to create connection, so need to pending Packets!
		return false;
	}

	HPendingConnectionStateToReceive* FoundPendingConnectionState = nullptr;
	for (auto& PendingConnectionState : PendingConnectionStatesToReceive)
	{
		if (PendingConnectionState.ClientConnection->SocketDesc == InPacket.SocketDesc)
		{
			FoundPendingConnectionState = &PendingConnectionState;
			break;
		}
	}

	// process the receive packets for pending connection state
	if (FoundPendingConnectionState == nullptr)
	{
		HPendingConnectionStateToReceive PendingConnectionState;
		PendingConnectionState.ClientConnection = NetConnection;

		// add the pending connection
		PendingConnectionStatesToReceive.push_back(PendingConnectionState);

		FoundPendingConnectionState = &PendingConnectionStatesToReceive.back();
	}
	
	FoundPendingConnectionState->ReceivePackets.push_back(InPacket);

	// successfully done!
	return true;
}
