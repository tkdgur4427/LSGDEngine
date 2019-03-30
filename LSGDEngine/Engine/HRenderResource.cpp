#include "HEnginePCH.h"
#include "HRenderResource.h"

using namespace lsgd;

HList<shared_ptr<HRenderResource>>* HRenderResource::GetResourceList()
{
	static HList<shared_ptr<HRenderResource>> List;
	return &List;
}

HCriticalSection* HRenderResource::GetSyncObject()
{
	static HCriticalSection SyncObject;
	return &SyncObject;
}

HRenderResource::~HRenderResource()
{
	check(bInitialized); // it should be called Release() first before calling the destructor
}

void HRenderResource::InitResource()
{
	HScopedLock Lock(*GetSyncObject());
	if (!bInitialized)
	{
		HList<shared_ptr<HRenderResource>>* ListRef = GetResourceList();
		ListRef->push_back(shared_ptr<HRenderResource>(this));

		InitDynamicRHI();
		InitRHI();

		bInitialized = true;
	}
}

void HRenderResource::ReleaseResource()
{
	HScopedLock Lock(*GetSyncObject());
	if (bInitialized)
	{
		HList<shared_ptr<HRenderResource>>* ListRef = GetResourceList();
		ListRef->remove(shared_ptr<HRenderResource>(this));

		ReleaseDynamicRHI();
		ReleaseRHI();

		bInitialized = false;
	}
}

void HRenderResource::UpdateRHI()
{
	HScopedLock Lock(*GetSyncObject());
	if (bInitialized)
	{
		ReleaseRHI();
		ReleaseDynamicRHI();
		InitDynamicRHI();
		InitRHI();		
	}
}

void BeginInitResource(shared_ptr<HRenderResource> Resource)
{
	ENQUEUE_UNIQUE_RENDER_COMMAND_ONE_PARAMETER(InitCommand, shared_ptr<HRenderResource>, ResourceParameter, Resource,
		{
			ResourceParameter->InitResource();
		});
}

void BeginUpdateResourceRHI(shared_ptr<HRenderResource> Resource)
{
	ENQUEUE_UNIQUE_RENDER_COMMAND_ONE_PARAMETER(UpdateCommand, shared_ptr<HRenderResource>, ResourceParameter, Resource,
		{
			ResourceParameter->UpdateRHI();
		});
}

void BeginReleaseResource(shared_ptr<HRenderResource> Resource)
{
	ENQUEUE_UNIQUE_RENDER_COMMAND_ONE_PARAMETER(ReleaseCommand, shared_ptr<HRenderResource>, ResourceParameter, Resource,
		{
			ResourceParameter->ReleaseResource();
		});
}