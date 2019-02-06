#include "HEnginePCH.h"
#include "HShaderFormatD3D12.h"

using namespace lsgd;

// function ptr declarations
typedef HRESULT (*pD3DCompile)(LPCVOID pSrcData, SIZE_T SrcDataSize, LPCSTR pSourceName, const D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, LPCSTR pEntryPoint, LPCSTR pTarget
	, UINT Flags1, UINT Flags2, ID3DBlob** ppCode, ID3DBlob** ppErrorMsgs);

typedef HRESULT (*pD3DReflect)(LPCVOID pSrcData, SIZE_T SrcDataSize, REFIID pInterface, void** ppReflector);

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

void HShaderFormatD3D12::CompileShader(const HShaderCompilerInput& Input, HShaderCompilerOutput& Output, const HString& WorkingDirectory)
{
	HRefCountPtr<ID3DBlob> Shader;
	HRefCountPtr<ID3DBlob> Errors;

	HRESULT Result;
	pD3DCompile D3DCompileFunc;
	pD3DReflect D3DReflectFunc;

	HString CompilerPath; //... need to specifies

	bool bCompilerPathFunctionUsed = GetD3DCompilerFuncs(CompilerPath, &D3DCompileFunc, &D3DReflectFunc);

	if (D3DCompileFunc)
	{
		//Result = D3DCompileFunc()
	}

	HShaderCompilerDefinitions AdditionalDefines;
	// AdditionalDefines.SetDefine("SM5_PROFILE", 1);
}
