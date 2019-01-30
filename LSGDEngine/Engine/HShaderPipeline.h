#pragma once

#include "HShader.h"

namespace lsgd
{
	// binding of a set of shader stages in a single pipeline
	class HShaderPipelineType
	{
	public:
		HString Name;
		
		// pipeline stages, ordered from lowest (usually PS) to highest (VS); guaranteed at least one stage 
		HArray<const class HShaderType*> Stages;

		const class HShaderType* AllStages[HShaderFrequency::SF_NumFrequency];
	};

	// a shader pipeline instance with compiled stages
	class HShaderPipeline
	{
		const HShaderPipelineType* PipelineType;
		HRefCountPtr<HShader> VertexShader;
		HRefCountPtr<HShader> PixelShader;
	};
}