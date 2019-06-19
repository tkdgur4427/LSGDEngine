#include "HEnginePCH.h"

// launch engine loop
#include "HEngineLoop.h"

using namespace lsgd;

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, PSTR lpCmdLine, INT nCmdShow)
{
	HEngineLoop EngineLoop;
	EngineLoop.Init();
	EngineLoop.Loop();
	EngineLoop.Destroy();

	return 0;
}