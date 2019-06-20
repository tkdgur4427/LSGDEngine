#include "HCorePCH.h"
#include "HSimpleConfig.h"
using namespace lsgd;

void HSimpleConfig::Load()
{
	(*this) << ClassRedirectors;
}