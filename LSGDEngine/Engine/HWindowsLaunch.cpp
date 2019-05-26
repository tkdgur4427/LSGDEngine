#include "HEnginePCH.h"

#include <windows.h>

// launch engine loop
#include "HEngineLoop.h"

//@todo - temporary
#include "../Memory/HBlockAllocator.h"
#include "../Core/HFrameProduceConsumeBuffer.h"

using namespace lsgd;

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, PSTR lpCmdLine, INT nCmdShow)
{
	containers::HFrameProduceConsumeBuffer<4096> Buffer;
	Buffer.Produce(255);
	Buffer.Produce(240);
	Buffer.Consume(255);
	Buffer.Consume(240);

	Buffer.Produce(24);
	Buffer.Produce(30);
	Buffer.Reset(1);

	HEngineLoop EngineLoop;
	EngineLoop.Init();
	EngineLoop.Loop();
	EngineLoop.Destroy();

	return 0;
}