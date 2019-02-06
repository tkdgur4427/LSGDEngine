#include "HEnginePCH.h"
#include "HShaderFormat.h"

// directx 12
#include "HShaderFormatD3D12.h"

using namespace lsgd;

HShaderFormat* HShaderFormat::FindOrCreateShaderFormat(const HString& ShaderFormat)
{
	static HHashMap<HString, HShaderFormat*> ShaderFormats;

	auto ResultIter = ShaderFormats.find(ShaderFormat);
	if (ResultIter == ShaderFormats.end())
	{
		HShaderFormat* NewShaderFormat = nullptr;
		if (ShaderFormat == "SM5")
		{
			NewShaderFormat = new HShaderFormatD3D12();
		}
		check(NewShaderFormat);

		ShaderFormats.insert({ShaderFormat, NewShaderFormat});

		ResultIter = ShaderFormats.find(ShaderFormat);
		check(ResultIter != ShaderFormats.end());
	}

	return ResultIter->second;
}