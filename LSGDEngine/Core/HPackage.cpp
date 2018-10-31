#include "HCorePCH.h"
#include "HPackage.h"

using namespace lsgd;

static HPackage GlobalTransientPackage;
HPackage* GTransientPackage = &GlobalTransientPackage;
