#include "HEnginePCH.h"
#include "HShaderCompilerEnvironment.h"

using namespace lsgd;

void HShaderCompilerEnvironment::SetDefine(const HString& Key, bool Value)
{
	HString StringValue = Value ? "1" : "0";
	Definitions.Definitions.insert({ Key, StringValue });
}

void HShaderCompilerEnvironment::SetDefine(const HString& Key, const HString& Value)
{
	Definitions.Definitions.insert({ Key, Value });
}