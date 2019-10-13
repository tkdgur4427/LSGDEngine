#include "HEnginePCH.h"
#include "HEditorEngine.h"
#include "HDynamicRHI.h"

using namespace lsgd;

void HEditorEngine::Initialize()
{
	HBaseEngine::Initialize();

	ImGuiPlatform = make_unique<HImGuiPlatform>();
	ImGuiPlatform->Initialize(WindowFrame->GetHandle());
}

void HEditorEngine::Destroy()
{
	HBaseEngine::Destroy();
}

void HEditorEngine::Tick(float DeltaTime)
{
	SGD_SCOPED_SIMPLE_PROFILER(HEditorEngine_Tick)

	HBaseEngine::Tick(DeltaTime);

	if (bWindowFrameMessageUpdated)
	{
		return;
	}

	// update ImGui
	ImGuiPlatform->Tick();

	{
		SGD_SCOPED_SIMPLE_PROFILER(ImGuiPlatform_Render)
		GDynamicRHI->RenderBegin();
		ImGuiPlatform->Render();
		GDynamicRHI->RenderEnd();
	}
}