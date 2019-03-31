#include "HEnginePCH.h"
#include "HImGuiPlatform.h"

#include "HImGuiDeviceWindows.h"
#include "HImGuiRenderDeviceDirectX12.h"

using namespace lsgd;

HImGuiPlatform::HImGuiPlatform()
{

}

HImGuiPlatform::~HImGuiPlatform()
{

}

bool HImGuiPlatform::Initialize(void* InHandle)
{
	// setup imgui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// setup imgui style
	ImGui::StyleColorsDark();

	//@todo - temporary no platform configuration
	ImGuiDevice = make_unique<HImGuiDevice, HImGuiDeviceWindows>();
	ImGuiDevice->Initialize(InHandle);

	ImGuiRenderDevice = make_unique<HImGuiRenderDevice, HImGuiRenderDeviceDirectX12>();
	ImGuiRenderDevice->Initialize();

	return true;
}

void HImGuiPlatform::Tick()
{
	ImGuiRenderDevice->Tick();
	ImGuiDevice->Tick();
	ImGui::NewFrame();

	bool show_demo_window = true;
	ImGui::ShowDemoWindow(&show_demo_window);
}

void HImGuiPlatform::Render()
{
	ImGuiRenderDevice->Render();
}