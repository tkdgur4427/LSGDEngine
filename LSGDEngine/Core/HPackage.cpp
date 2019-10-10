#include "HCorePCH.h"
#include "HPackage.h"

namespace lsgd
{
	static HTranscientPackageForTest GlobalTransientPackage;
	HPackage* GTransientPackage = &GlobalTransientPackage;

	HPackage* GPersistentPackage = &GlobalTransientPackage;
}

using namespace lsgd;

HTranscientPackageForTest::HTranscientPackageForTest()
{
	// generate package name
	Name = HName("GTranscientPackageForTest");
}
