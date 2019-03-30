#pragma once

namespace lsgd
{
	class HRenderResource : public enable_shared_from_this<HRenderResource>
	{
	public:
		static HList<shared_ptr<HRenderResource>>* GetResourceList();
		HCriticalSection* GetSyncObject();

		HRenderResource()
			: bInitialized(false)
		{}

		virtual ~HRenderResource();

		virtual void InitDynamicRHI() {}
		virtual void ReleaseDynamicRHI() {}

		virtual void InitRHI() {}
		virtual void ReleaseRHI() {}

		virtual void InitResource();
		virtual void ReleaseResource();
		virtual void UpdateRHI();

		bool bInitialized;
	};
}

extern void BeginInitResource(shared_ptr<HRenderResource> Resource);
extern void BeginUpdateResourceRHI(shared_ptr<HRenderResource> Resource);
extern void BeginReleaseResource(shared_ptr<HRenderResource> Resource);
