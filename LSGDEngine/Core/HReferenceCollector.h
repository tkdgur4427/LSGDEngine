#pragma once

#include "HReflectionContext.h"

using namespace lsgd::reflect;

namespace lsgd { namespace gc {

	class HReferenceCollector : public HReflectionContext
	{
	public:
		HReferenceCollector(HArray<HObject*>& InRootSet)
			: RootSet(InRootSet)
		{}

		// CollectGC (Marking GC)
		void Collect();
				
		virtual HReflectionContext& operator<<(class HObject*& Value) override;

		// root-set
		HArray<HObject*>& RootSet;
	};

} }