#include "HEnginePCH.h"
#include "HShaderId.h"

using namespace lsgd;

HShaderId::HShaderId(const HShaderTarget& InShaderTarget, HVertexFactoryType* InVertexFactoryType, HShaderType* InShaderType)
	: Target(InShaderTarget)
	, ShaderType(InShaderType)
	, VertexFactoryType(InVertexFactoryType)
{

}

HShaderId::~HShaderId()
{

}