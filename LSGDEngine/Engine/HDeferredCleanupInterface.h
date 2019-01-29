#pragma once

namespace lsgd {

	class HDeferredCleanupInterface
	{
	public:
		virtual void FinishCleanup() = 0;
		virtual ~HDeferredCleanupInterface() {}
	};

	// a set of clean-up objects which are pending deletion
	class HPendingCleanupObjects
	{
		HArray<HDeferredCleanupInterface*> CleanupArray;

	public:
		HPendingCleanupObjects();
		~HPendingCleanupObjects();
	};	
}

extern void BeginCleanup(HDeferredCleanupInterface* CleanupObject);