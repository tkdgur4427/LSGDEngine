#include "HEnginePCH.h"
#include "HShaderFormatD3D12.h"

#include "HShaderCompilingManager.h"

using namespace lsgd;

// function ptr declarations
typedef HRESULT (__stdcall *pD3DCompile)(LPCVOID pSrcData, SIZE_T SrcDataSize, LPCSTR pSourceName, const D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, LPCSTR pEntryPoint, LPCSTR pTarget
	, UINT Flags1, UINT Flags2, ID3DBlob** ppCode, ID3DBlob** ppErrorMsgs);

typedef HRESULT (__stdcall *pD3DReflect)(LPCVOID pSrcData, SIZE_T SrcDataSize, REFIID pInterface, void** ppReflector);

// gets function pointers from the dll at NewCompilerPath
bool GetD3DCompilerFuncs(const HString& NewCompilerPath, pD3DCompile* OutD3DCompile, pD3DReflect* OutD3DReflect)
{
	static HString CurrentCompiler;
	static HMODULE CompilerDLL = 0;

	if (CurrentCompiler != NewCompilerPath)
	{
		CurrentCompiler = NewCompilerPath;
		if (CompilerDLL)
		{
			FreeLibrary(CompilerDLL);
			CompilerDLL = 0;
		}

		if (CurrentCompiler.length())
		{
			CompilerDLL = LoadLibrary(CurrentCompiler.data());

			// @todo - temporary
			if (CompilerDLL == 0)
			{
				DWORD ErrorCode = 0;
				ErrorCode = GetLastError();
			}
		}

		if (!CompilerDLL && NewCompilerPath.length())
		{
			// couldn't find HLSL compiler in specified path; we fail the first compile
			*OutD3DCompile = 0;
			*OutD3DReflect = 0;
			return false;
		}
	}

	if (CompilerDLL)
	{
		// from custom folder; e.g. C:/DX8/D3DCompiler_44.dll
		*OutD3DCompile = (pD3DCompile)(void*)GetProcAddress(CompilerDLL, "D3DCompile");
		*OutD3DReflect = (pD3DReflect)(void*)GetProcAddress(CompilerDLL, "D3DReflect");
		return true;
	}

	//...

	return false;
}

HString HShaderFormatD3D12::GetShaderProfile(HShaderFrequency Frequency)
{
	HString Result;
	switch (Frequency)
	{
	case HShaderFrequency::SF_Vertex:
		Result = "vs_5_0";
		break;
	case HShaderFrequency::SF_Pixel:
		Result = "ps_5_0";
		break;
	}
	return Result;
}

void HShaderFormatD3D12::CompileShader(const HShaderCompilerInput& Input, HShaderCompilerOutput& Output, const HString& WorkingDirectory)
{
	HString PreprocessedShaderSource;

	// @todo - temporary read the shader file~
	HString ShaderSourceFilePath = WorkingDirectory + Input.SourceFilename;
	unique_ptr<HPlatformFileIO> PlatformFileIO = (HGenericPlatformMisc::CreatePlatformFileIO());
	PlatformFileIO->OpenFile(ShaderSourceFilePath, EFileUsageFlag::Read);
	
	const int32 StrBufferMax = 1024;
	char StringBuffer[StrBufferMax];

	bool bEOF = false;
	while (!bEOF)
	{
		int64 StrSizeRead = 0;
		PlatformFileIO->ReadFile(&StringBuffer[0], StrBufferMax, StrSizeRead);

		// append to the PreprocessedShaderSource
		PreprocessedShaderSource.append(StringBuffer, StrSizeRead);

		// it means that reaching to the end of EOF
		if (StrBufferMax != (int32)StrSizeRead)
		{
			break;
		}
	}

	PlatformFileIO->CloseFile();

	HRefCountPtr<ID3DBlob> Shader;
	HRefCountPtr<ID3DBlob> Errors;

	HRESULT Result;
	pD3DCompile D3DCompileFunc;
	pD3DReflect D3DReflectFunc;

	HString AdditionalD3DCompilerPath = "x86//d3dcompiler_47.dll";
	HString CompilerPath = HGenericPlatformMisc::GetD3DDir() + AdditionalD3DCompilerPath;

	bool bCompilerPathFunctionUsed = GetD3DCompilerFuncs(CompilerPath, &D3DCompileFunc, &D3DReflectFunc);

	// get the shader profile
	HString ShaderProfile = GetShaderProfile((HShaderFrequency)Input.Target.Frequency);

	if (D3DCompileFunc)
	{
		Result = D3DCompileFunc(
			PreprocessedShaderSource.data(),
			PreprocessedShaderSource.length(),
			Input.SourceFilename.data(),
			nullptr,
			nullptr,
			Input.EntryPointName.data(),
			ShaderProfile.data(),
			0,
			0,
			Shader.GetInitReference(),
			Errors.GetInitReference()
		);
	}
	
	// gather reflection information
	int32 NumInterpolants = 0;
	HArray<HString> InterpolantNames;
	HArray<HString> ShaderInputs;

	if (SUCCEEDED(Result))
	{
		if (D3DReflectFunc)
		{
			Output.bSuccessed = true;
			ID3D11ShaderReflection* Reflector = nullptr;

			// IID_ID3D11ShaderReflectionCurrentCompiler is defined in this file and needs to match the IID from the dll in CompilerPath
			// if the function pointers from that dll are being used
			const IID ShaderReflectionID = IID_ID3D11ShaderReflection;
			Result = D3DReflectFunc(Shader->GetBufferPointer(), Shader->GetBufferSize(), ShaderReflectionID, (void**)&Reflector);

			if (FAILED(Result))
			{
				check(false);
			}

			// read the constant table description
			D3D11_SHADER_DESC ShaderDesc;
			Reflector->GetDesc(&ShaderDesc);

			bool bGlobalUniformBufferUsed = false;
			uint32 NumSamplers = 0;
			uint32 NumSRVs = 0;
			uint32 NumCBs = 0;
			uint32 NumUAVs = 0;
			HArray<HString> UniformBufferNames;
			HArray<HString> ShaderOutputs;

			if (Input.Target.Frequency == SF_Vertex)
			{
				for (uint32 Index = 0; Index < ShaderDesc.OutputParameters; ++Index)
				{
					// VC++ horrible hack; runtime esp checks get confused and fail for some reason calling Reflector->GetOutputParameterDesc() (because it comes from another DLL?)
					// so 'guard it' using the middle of an array; it's been confirmed No corruption is really happening
					D3D11_SIGNATURE_PARAMETER_DESC ParamDescs[3];
					D3D11_SIGNATURE_PARAMETER_DESC& ParamDesc = ParamDescs[1];
					Reflector->GetOutputParameterDesc(Index, &ParamDesc);

					if (ParamDesc.SystemValueType == D3D_NAME_UNDEFINED && ParamDesc.Mask != 0)
					{
						++NumInterpolants;
						HString NewInterpolantName = HStringPrintf("%s%d", ParamDesc.SemanticName, ParamDesc.SemanticIndex);
						InterpolantNames.push_back(NewInterpolantName);
					}
				}
			}

			else if (Input.Target.Frequency == SF_Pixel)
			{
				bool bFoundUnused = false;
				for (uint32 Index = 0; Index < ShaderDesc.InputParameters; ++Index)
				{
					D3D11_SIGNATURE_PARAMETER_DESC ParamDescs[3];
					D3D11_SIGNATURE_PARAMETER_DESC& ParamDesc = ParamDescs[1];
					Reflector->GetInputParameterDesc(Index, &ParamDesc);

					if (ParamDesc.SystemValueType == D3D_NAME_UNDEFINED)
					{
						if (ParamDesc.ReadWriteMask != 0)
						{
							HString SemanticName = ParamDesc.SemanticName;
							HAddUnique(ShaderInputs, SemanticName);

							// add the number (for the case of TEXCOORD)
							HString SemanticIndexName = ParamDesc.SemanticName;
							HAddUnique(ShaderInputs, SemanticIndexName);

							// add _centroid
							//...
						}
						else
						{
							bFoundUnused = true;
						}
					}
					else
					{
						HString SemanticName = ParamDesc.SemanticName;
						ShaderInputs.push_back(SemanticName);
					}
				}

				//...
			}

			// add parameters for shader resources (CB, Tex, Samplers, etc)
			for (uint32 ResourceIndex = 0; ResourceIndex < ShaderDesc.BoundResources; ResourceIndex++)
			{
				D3D11_SHADER_INPUT_BIND_DESC BindDesc;
				Reflector->GetResourceBindingDesc(ResourceIndex, &BindDesc);

				if (BindDesc.Type == D3D10_SIT_CBUFFER || BindDesc.Type == D3D10_SIT_TBUFFER)
				{
					const uint32 CBIndex = BindDesc.BindPoint;
					ID3D11ShaderReflectionConstantBuffer* ConstantBuffer = Reflector->GetConstantBufferByIndex(CBIndex);
					D3D11_SHADER_BUFFER_DESC CBDesc;
					ConstantBuffer->GetDesc(&CBDesc);
					bool bGlobalCB = HStringUtil::Strcmp(CBDesc.Name, "$Globals");

					if (bGlobalCB)
					{
						// track all of the variables in this constant buffer
						for (uint32 ConstantIndex = 0; ConstantIndex < CBDesc.Variables; ConstantIndex++)
						{
							ID3D11ShaderReflectionVariable* Variable = ConstantBuffer->GetVariableByIndex(ConstantIndex);
							D3D11_SHADER_VARIABLE_DESC VariableDesc;
							Variable->GetDesc(&VariableDesc);

							if (VariableDesc.uFlags & D3D10_SVF_USED)
							{
								bGlobalUniformBufferUsed = true;

								Output.ParameterMap.AddParameterAllocation(VariableDesc.Name, CBIndex, VariableDesc.StartOffset, VariableDesc.Size);
								//UnusedUniformBufferSlots[CBIndex] = true;
							}
						}
					}
					else
					{
						// track just the constant buffer itself
						Output.ParameterMap.AddParameterAllocation(
							CBDesc.Name,
							CBIndex,
							0,
							0
						);
						//UnusedUniformBufferSlots[CBIndex] = true;

						if (UniformBufferNames.size() <= (int32)CBIndex)
						{
							for (int32 Index = UniformBufferNames.size() - 1; Index < CBIndex; ++Index)
							{
								UniformBufferNames.push_back("");
							}
						}
						UniformBufferNames[CBIndex] = CBDesc.Name;
					}

					NumCBs = Max<uint32>(NumCBs, BindDesc.BindPoint + BindDesc.BindCount);
				}
				else if (BindDesc.Type == D3D10_SIT_TEXTURE || BindDesc.Type == D3D10_SIT_SAMPLER)
				{
					char OfficialName[1024];
					uint32 BindCount = BindDesc.BindCount;
					HStringUtil::Strcpy(OfficialName, BindDesc.Name);

					// assign the name and optionally strip any [#] suffixes
					const char* BracketLocation = HStringUtil::Strchr(OfficialName, '[');
					if (BracketLocation)
					{
						BindCount = 1;

						// this needs to include the first [ character otherwise it will include now array textures with matching starting characters
						// e.g. 'LightMapTexturesTest' which is not part of 'LightMapTextures[#] would be included as the last index of 'LightMapTextures'
						const int32 NumCharacterToCompare = BracketLocation - OfficialName + 1;

						/*
							in SM5, for some reason, array suffixes are included in Name, i.e. 'LightMapTextures'
							additionally elements in an array are listed as SEPARATE bound resources
							however, they are always contiguous in resource index, so iterate over the samplers and textures of the initial association
							and count them, identifying the bind point and bind points
						*/
						while (ResourceIndex + 1 < ShaderDesc.BoundResources)
						{
							D3D11_SHADER_INPUT_BIND_DESC BindDesc2;
							Reflector->GetResourceBindingDesc(ResourceIndex + 1, &BindDesc2);

							if (BindDesc2.Type == BindDesc.Type && HStringUtil::Strncmp(BindDesc2.Name, BindDesc.Name, NumCharacterToCompare) == 0)
							{
								BindCount++;
								// skip over this resource since it is part of an array
								ResourceIndex++;
							}
							else
							{
								break;
							}
						}
					}

					if (BindDesc.Type == D3D10_SIT_SAMPLER)
					{
						NumSamplers = Max<uint32>(NumSamplers, BindDesc.BindPoint + BindCount);
					}
					else
					{
						NumSRVs = Max<uint32>(NumSRVs, BindDesc.BindPoint + BindCount);
					}

					// add a parameter for the texture only, the sampler index will be invalid
					Output.ParameterMap.AddParameterAllocation(
						OfficialName,
						0,
						BindDesc.BindPoint,
						BindCount
					);
				}
				else if (BindDesc.Type == D3D11_SIT_UAV_RWTYPED
					|| BindDesc.Type == D3D11_SIT_UAV_RWSTRUCTURED
					|| BindDesc.Type == D3D11_SIT_UAV_RWBYTEADDRESS
					|| BindDesc.Type == D3D11_SIT_UAV_RWSTRUCTURED_WITH_COUNTER
					|| BindDesc.Type == D3D11_SIT_UAV_APPEND_STRUCTURED)
				{
					Output.ParameterMap.AddParameterAllocation(
						BindDesc.Name,
						0,
						BindDesc.BindPoint,
						1
					);

					NumUAVs = Max<uint32>(NumUAVs, BindDesc.BindPoint + BindDesc.BindCount);
				}
				else if (BindDesc.Type == D3D11_SIT_STRUCTURED || D3D11_SIT_BYTEADDRESS)
				{
					Output.ParameterMap.AddParameterAllocation(
						BindDesc.Name,
						0,
						BindDesc.BindPoint,
						1
					);

					NumUAVs = Max<uint32>(NumSRVs, BindDesc.BindPoint + BindDesc.BindCount);
				}
			}

			HRefCountPtr<ID3DBlob> CompressedData;
			//if () {}
			///else
			{
				 // D3DStripShader is not guaranteed to exist
				// e.g. the open-source DXIL shader compiler does not currently implement it
				CompressedData = Shader;
			}

			// build the SRT for this shader
			
		}
	}
}
