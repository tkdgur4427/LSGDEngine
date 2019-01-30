#pragma once

namespace lsgd
{
	enum HShaderFrequency
	{
		SF_Vertex,
		SF_Pixel,
		SF_NumFrequency,
	};

	enum HShaderPlatform
	{
		SP_SM5,
	};

	struct HShaderTarget
	{
		uint32 Frequency;
		uint32 Platform;
	};
}