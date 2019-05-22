#include "HEnginePCH.h"

#include <windows.h>

// launch engine loop
#include "HEngineLoop.h"

//@todo - temporary
#include "../Memory/HBlockAllocator.h"

using namespace lsgd;

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, PSTR lpCmdLine, INT nCmdShow)
{
	allocators::HBlockAllocator<> BlockAllocator;
	void* NewMemory = BlockAllocator.AllocateMemoryBlock();
	BlockAllocator.DeallocateMemoryBlock(NewMemory);

	HEngineLoop EngineLoop;
	EngineLoop.Init();
	EngineLoop.Loop();
	EngineLoop.Destroy();

	return 0;
}