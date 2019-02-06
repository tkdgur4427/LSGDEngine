#include "HEnginePCH.h"
#include "HEngineLoop.h"

// async
#include "..\Core\HTaskGraphInterface.h"
#include "..\Core\HTaskThreadBase.h"

// gc
#include "..\Core\HGarbageCollect.h"

// ebus
#include "HEBus.h"

// world
#include "HWorld.h"

// engine instance
#include "HEditorEngine.h"

// rendering thread
#include "HRenderingThread.h"

// shader compiling manager
#include "HShaderCompilingManager.h"

// shader compiler util
#include "HShaderCompilerUtil.h"

using namespace lsgd;
using namespace lsgd::async;

HEngineLoop::HEngineLoop()
	: bTerminate(false)
{
	
}

HEngineLoop::~HEngineLoop()
{

}

void HEngineLoop::Init()
{
	// reflection post update
	reflect::HTypeDatabase::GetSingleton()->ProcessPostProcessClassData();

	// create named threads

	// 1. rendering thread
	InitializeRenderingThread();

	// create task graph
	TaskGraph = make_unique<HTaskGraphInterface, HTaskGraph1>();
	TaskGraph->Initialize();

	// create shader compiling manager
	ShaderCompilingManager = make_unique<HShaderCompilingManager>();
	ShaderCompilingManager->Initialize();

	GShaderCompilerManager = ShaderCompilingManager.get();

	// shader type initializations
	HShaderCompilerUtil::InitializeShaderTypes();

	HShaderCompilerUtil::CompileGlobalShaderMap(HShaderPlatform::SP_SM5, false);
}

class HEngineLoopInitializeTask
{
public:
	HEngineLoopInitializeTask(HEngineLoop& InEngineLoop)
		: EngineLoopRef(InEngineLoop)
	{}

	void Execute()
	{
		EngineLoopRef.InitializeInNamedThread();
	}

	HEngineLoop& EngineLoopRef;
};

class HEngineLoopTickTask
{
public:
	HEngineLoopTickTask(HEngineLoop& InEngineLoop)
		: EngineLoopRef(InEngineLoop)
	{}

	void Execute()
	{
		EngineLoopRef.Tick();
	}

	HEngineLoop& EngineLoopRef;
};

void HEngineLoop::Loop()
{
	// create named thread for main thread
	HTaskThreadBase::TaskThreadSharedContext.CreateNamedThread("MainThread");
	shared_ptr<HThreadRunnable> MainThreadRunnable = HTaskThreadBase::TaskThreadSharedContext.GetNamedThread("MainThread");

	// convert to thread task base
	HNamedTaskThread* RawMainThreadRunnabe = (HNamedTaskThread*)MainThreadRunnable.get();

	// initialize in named thread
	HArray<shared_ptr<HGraphEvent>> Prerequisites;
	shared_ptr<HGraphEvent> InitializeEvent = HGraphTask<HEngineLoopInitializeTask>::CreateTask(Prerequisites, true, "MainThread").ConstructAndDispatchWhenReady(*this);

	while (!RawMainThreadRunnabe->IsTerminated())
	{
		// create task
		HArray<shared_ptr<HGraphEvent>> Prerequisites;
		Prerequisites.push_back(InitializeEvent);
		shared_ptr<HGraphEvent> GraphEvent = HGraphTask<HEngineLoopTickTask>::CreateTask(Prerequisites, true, "MainThread").ConstructAndDispatchWhenReady(*this);

		// wait until complete the task 'HEngineLoopTickTask'		
		TaskGraph->WaitUntilTasksComplete(Prerequisites, true, "MainThread");
	}
}

void HEngineLoop::Destroy()
{
	// destroy shader compiling manager
	ShaderCompilingManager->Destroy();

	DestroyRenderingThread();	

	// destroy game instance
	// @todo think about how to handle this in named thread...
	EngineInstance->Destroy();

	// destroy the task-graph
	TaskGraph->Destroy();
}

void HEngineLoop::InitializeInNamedThread()
{
	// engine instance
	EngineInstance = make_unique<HBaseEngine, HEditorEngine>();
	EngineInstance->Initialize();
}

void HEngineLoop::Tick()
{
	EngineInstance->Tick(0.f);

	// world tick
	if (GWorld)
	{
		GWorld->Tick(0.f);
	}

	// trigger gc
	gc::HGarbageCollect GarbageCollect;
	//GarbageCollect.MarkAndSweep();
}
