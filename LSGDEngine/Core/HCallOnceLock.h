#pragma once

namespace lsgd { namespace thread {

	// helper class for calling call_once implementation	
	class HCallOnceLock
	{
	public:
		template <typename InFunctionType, typename... Args>
		HCallOnceLock(HCriticalSectionCallOnce& SyncObjectRef, InFunctionType&& InCallable, Args&&... InArguments)
		{
			HCallOnce(SyncObjectRef, InCallable, InArguments...);
		}

		~HCallOnceLock() {}

	protected:
	};

}}