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
}

void HIpDriver::HandleListenerConnectionAccepted(const HString& SocketDesc)
{
	// add new connection to HIpDriver

}

void HIpDriver::DisconnectClient(const HString& SocketDesc)
{
	Implementation->DisconnectSocket(SocketDesc);
}