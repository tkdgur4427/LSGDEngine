#include "HEnginePCH.h"
#include "HImGuiDevice.h"

HImGuiDevice::HImGuiDevice()
	: Time(0)
	, TicksPerSecond(0)
	, LastMouseCursor(ImGuiMouseCursor_COUNT)
	, Handle(nullptr)
{

}

HImGuiDevice::~HImGuiDevice()
{

}