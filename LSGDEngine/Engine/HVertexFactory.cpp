#include "HEnginePCH.h"
#include "HVertexFactory.h"

using namespace lsgd;

HVertexFactoryType::HVertexFactoryType()
{
	AddToGlobalList(this);
}

HVertexFactoryType::~HVertexFactoryType()
{
	RemvoeFromGlobalList(this);
}