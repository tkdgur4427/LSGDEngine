#include "HEnginePCH.h"
#include "HRenderingThread.h"

using namespace lsgd;
using namespace lsgd::async;

static shared_ptr<HThreadRunnable> RenderingThreadRunnable;

void InitializeRenderingThread()
{
	HTaskThreadBase::TaskThreadSharedContext.CreateNamedThread("RenderingThread");
	RenderingThreadRunnable = HTaskThreadBase::TaskThreadSharedContext.GetNamedThread("RenderingThread");
}

void DestroyRenderingThread()
{
	// release the rendering thread runnable
	RenderingThreadRunnable.reset();
}
