#include "HEnginePCH.h"
#include "HImGuiUtil.h"

using namespace lsgd;

void HImGuiUtil::InitializeImGui()
{
	// setup imgui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// setup imgui style
	ImGui::StyleColorsDark();

	// setup platform/renderer bindings

}