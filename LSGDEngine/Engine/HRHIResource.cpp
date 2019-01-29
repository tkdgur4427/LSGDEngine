#include "HEnginePCH.h"
#include "HRHIResource.h"

using namespace lsgd;

HConcurrentQueue<HRHIResource*> HRHIResource::PendingDeletes;
HRHIResource* HRHIResource::CurrentlyDeleting;
HArray<HRHIResource::ResourceToDelete> HRHIResource::DeferredDeletionQueue;
uint32 HRHIResource::CurrentFrame = 0;

void HRHIResource::FlushPendingDeletes(bool bFlushDeferredDeletes)
{
	auto Delete = [](HArray<HRHIResource*>& ToDelete)
	{
		for (auto& DeleteResource : ToDelete)
		{
			if (DeleteResource->GetRefCount() == 0)
			{
				CurrentlyDeleting = DeleteResource;
				delete DeleteResource;
				CurrentlyDeleting = nullptr;
			}
			else
			{
				DeleteResource->MarkedForDelete = 0;
			}
		}
	};

	while (1)
	{
		if (PendingDeletes.Num() == 0)
		{
			break;
		}

		HArray<HRHIResource*> ToDelete;
		PendingDeletes.PopAll(ToDelete);
		Delete(ToDelete);

		const uint32 NumFramesToExpire = 3;

		if (DeferredDeletionQueue.size())
		{
			int32 DeletedBatchCount = 0;
			while (DeletedBatchCount < int32(DeferredDeletionQueue.size()))
			{
				ResourceToDelete& ResourceBatch = DeferredDeletionQueue[DeletedBatchCount];
				if (((ResourceBatch.FrameDeleted + NumFramesToExpire) < CurrentFrame))
				{
					Delete(ResourceBatch.Resources);
					++DeletedBatchCount;
				}
				else
				{
					break;
				}
			}

			if (DeletedBatchCount)
			{
				auto StartPos = DeferredDeletionQueue.begin();
				DeferredDeletionQueue.erase(StartPos, StartPos + (DeletedBatchCount - 1));
			}

			++CurrentFrame;
		}
	}
}