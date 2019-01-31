#include "HEnginePCH.h"
#include "HShaderResource.h"
#include "HRHIResource.h"

using namespace lsgd;

HShaderResource::HShaderResource()
	: NumRefs(0)
{

}

HShaderResource::~HShaderResource()
{
	check(NumRefs == 0);
}