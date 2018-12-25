#include "HEnginePCH.h"
#include "HEngineLoop.h"

// async
#include "..\Core\HTaskGraphInterface.h"
#include "..\Core\HTaskThreadBase.h"

// gc
#include "..\Core\HGarbageCollect.h"

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
	//debugf("ProcessPostProcessClassData[%d]", 0);

	// create named threads

	// 1. rendering thread
	HTaskThreadBase::TaskThreadSharedContext.CreateNamedThread("RenderingThread");

	// create task graph
	TaskGraph = make_unique<HTaskGraphInterface, HTaskGraph1>();
	TaskGraph->Initialize();
}

void HEngineLoop::Loop()
{
	while (!bTerminate)
	{
		// trigger gc
		gc::HGarbageCollect GarbageCollect;
		//GarbageCollect.MarkAndSweep();
	}
}

void HEngineLoop::Destroy()
{
	// destroy the task-graph
	TaskGraph->Destroy();
}
