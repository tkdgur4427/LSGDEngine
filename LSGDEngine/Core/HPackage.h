#pragma once

#include "HObject.h"

namespace lsgd
{
	class HPackage
	{
	public:	
		HName Name;

		//@todo - add additional abstract hierarchical for the package
		// HPackage* Outer;
	};

	// @todo : it is just temporary package class for testing HLinker[Load/Save]
	class HTranscientPackageForTest : public HPackage
	{
	public:
		HTranscientPackageForTest();

		// all HObjects added to this transient package
		HArray<HObject*> TrackedObjects;
	};

	extern HPackage* GTransientPackage;
}