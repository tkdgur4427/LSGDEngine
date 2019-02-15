#pragma once

namespace lsgd
{
	// @todo

	// a uniform buffer struct
	class HUniformBufferStruct
	{
	public:
		// a member of a uniform buffer type
		class HMember
		{
		public:
			HString Name;
			HString ShaderType;
			uint32 Offset;
			HUniformBufferBaseType Type;
			uint32 NumRows;
			uint32 NumColumns;
			uint32 NumElements;
			const HUniformBufferStruct* Struct;
		};

		typedef class HShaderUniformBufferParameter* (*ConstructUniformBufferParameterType)();

		HString StructTypeName;
		HString ShaderVariableName;
		ConstructUniformBufferParameterType ConstructUniformBufferParameterRef;

		uint32 Size;
	};
}