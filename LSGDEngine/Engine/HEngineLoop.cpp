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
	HTaskThreadBase::TaskThreadSharedContext.CreateNamedThread("RenderingThread");

	// create task graph
	TaskGraph = make_unique<HTaskGraphInterface, HTaskGraph1>();
	TaskGraph->Initialize();
}

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

	while (!RawMainThreadRunnabe->IsTerminated())
	{
		// create task
		HArray<shared_ptr<HGraphEvent>> Prerequisites;
		shared_ptr<HGraphEvent> GraphEvent = HGraphTask<HEngineLoopTickTask>::CreateTask(Prerequisites, true, "MainThread").ConstructAndDispatchWhenReady(*this);

		// wait until complete the task 'HEngineLoopTickTask'		
		TaskGraph->WaitUntilTasksComplete(Prerequisites, true, "MainThread");
	}
}

void HEngineLoop::Destroy()
{
	// destroy the task-graph
	TaskGraph->Destroy();
}

void HEngineLoop::Tick()
{
	// world tick
	if (GWorld)
	{
		GWorld->Tick(0.f);
	}

	// trigger gc
	gc::HGarbageCollect GarbageCollect;
	//GarbageCollect.MarkAndSweep();
}
