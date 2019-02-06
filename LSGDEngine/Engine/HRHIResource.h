#pragma once

#include "../Core/HThreadSafeCounter.h"

using namespace lsgd::thread;
using namespace lsgd::container;

namespace lsgd
{
	class HRHIResource
	{
	public:
		HRHIResource(bool InbDoNotDeferDelete = false)
			: MarkedForDelete(0)
			, bDoNotDeferDelete(InbDoNotDeferDelete)
			, bCommitted(true)
		{}

		virtual ~HRHIResource()
		{
			check(NumRefs.GetValue() == 0 && (CurrentlyDeleting == this || bDoNotDeferDelete));
		}

		bool DeferDelete() const
		{
			return !bDoNotDeferDelete;
		}

		uint32 AddRef() const
		{
			int32 NewValue = NumRefs.Increment();
			return uint32(NewValue);
		}

		uint32 Release() const
		{
			int32 NewValue = NumRefs.Decrement();
			if (NewValue == 0)
			{
				if (!DeferDelete())
				{
					delete this;
				}
				else
				{
					if (HGenericPlatformAtomics::HInterlockedCompareSwap(&MarkedForDelete, 1, 0) == 0)
					{
						PendingDeletes.Push(const_cast<HRHIResource*>(this));
					}
				}
			}
			check(NewValue >= 0);
			return uint32(NewValue);
		}

		uint32 GetRefCount() const { return NumRefs.GetValue(); }

		void DoNotDeferDelete()
		{
			bDoNotDeferDelete = true;
		}

		static void FlushPendingDeletes(bool bFlushDeferredDeletes = false);

		mutable HThreadSafeCounter NumRefs;
		mutable int32 MarkedForDelete;
		volatile bool bDoNotDeferDelete;
		bool bCommitted;

		static HConcurrentQueue<HRHIResource*> PendingDeletes;
		static HRHIResource* CurrentlyDeleting;

		// some APIs don't do internal reference counting, so we have to wait an extra couple of frames before deleting resources
		// to ensure the GPU has completely finished with them; this avoids expensive fences, etc
		struct ResourceToDelete
		{
			ResourceToDelete(uint32 InFrameDeleted = 0)
				: FrameDeleted(InFrameDeleted)
			{}

			HArray<HRHIResource*> Resources;
			uint32 FrameDeleted;
		};

		static HArray<ResourceToDelete> DeferredDeletionQueue;
		static uint32 CurrentFrame;
	};

	// RHI resources
	class HRHIVertexDeclaration : public HRHIResource {};
	class HRHIShader : public HRHIResource
	{
	public:
		HString ShaderName;
	};

	class HRHIVertexShader : public HRHIShader {};
	class HRHIPixelShader : public HRHIShader {};
}