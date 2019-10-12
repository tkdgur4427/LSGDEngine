#include "HEnginePCH.h"
#include "HNetworkEvents.h"

// NetConnection
#include "HNetConnection.h"

using namespace lsgd;

void HNetworkEvents::Execute(HObjectHandleWeak<class HNetConnection> NetConnection, shared_ptr<HNetworkEvents> InNetworkEvents)
{
	if (NetConnection.IsValid())
	{
		InNetworkEvents->HandleEvent(NetConnection.Get());
	}
}

void HNetworkEventHandler::HandleEvent(class HNetConnection* InConnection)
{
	// trigger handle event in packet
	Packet->HandleEvent(InConnection);
}
