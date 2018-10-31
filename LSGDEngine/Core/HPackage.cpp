#include "HCorePCH.h"
#include "HPackage.h"

namespace lsgd
{
	static HPackage GlobalTransientPackage;
	HPackage* GTransientPackage = &GlobalTransientPackage;
}
