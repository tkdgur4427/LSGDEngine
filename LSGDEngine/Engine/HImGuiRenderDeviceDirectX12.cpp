#include "HEnginePCH.h"
#include "HImGuiRenderDeviceDirectX12.h"

#include "HDynamicRHI.h"
#include "HImGuiShaders.h"

using namespace lsgd;

// @todo - note that imgui module should be merged with RHI!
// {

struct HVertexConstantBuffer
{
	float MVP[4][4];
};

struct HFrameResources
{
	ID3D12Resource* IB;
	ID3D12Resource* VB;
	int32 VertexBufferSize;
	int32 IndexBufferSize;
};

static const uint32 NUM_FRAMES_INFLIGHT = 3;
static HFrameResources FrameResources[NUM_FRAMES_INFLIGHT] = {};

static ID3D12PipelineState* D3D12PipelineState = nullptr;
static ID3D12RootSignature* D3D12RootSignature = nullptr;
static ID3D10Blob* VertexShaderBlob = nullptr;
static ID3D10Blob* PixelShaderBlob = nullptr;
static DXGI_FORMAT RTVFormat = DXGI_FORMAT_UNKNOWN;
static ID3D12Resource* FontTextureResource = nullptr;
static D3D12_CPU_DESCRIPTOR_HANDLE FontSrvCpuDescHandle = {};
static D3D12_GPU_DESCRIPTOR_HANDLE FontSrvGpuDescHandle = {};

// }

bool HImGuiRenderDeviceDirectX12::Initialize()
{
	ImGuiIO& IO = ImGui::GetIO();
	IO.BackendRendererName = "imgui_impl_dx12";

	ID3D12Device* DeviceObject = (ID3D12Device*)GDynamicRHI->GetDevice();
	ID3D12DescriptorHeap* SrvDescriptorHeap = (ID3D12DescriptorHeap*)GDynamicRHI->GetSrvDescriptionHeap();

	RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	FontSrvCpuDescHandle = SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	FontSrvGpuDescHandle = SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

	// create buffers
	for (int32 Index = 0; Index < NUM_FRAMES_INFLIGHT; ++Index)
	{
		FrameResources[Index].IB = nullptr;
		FrameResources[Index].VB = nullptr;
		FrameResources[Index].VertexBufferSize = 5000;
		FrameResources[Index].IndexBufferSize = 10000;
	}

	return true;
}

void HImGuiRenderDeviceDirectX12::CreateFontsTexture()
{
	ID3D12Device* DeviceObject = (ID3D12Device*)GDynamicRHI->GetDevice();

	// build texture atlas
	ImGuiIO& IO = ImGui::GetIO();
	uint8* Pixels;
	int32 Width, Height;
	IO.Fonts->GetTexDataAsRGBA32(&Pixels, &Width, &Height);

	// upload texture to graphics system
	{
		D3D12_HEAP_PROPERTIES Props;
		HGenericMemory::MemZero(&Props, 0, sizeof(D3D12_HEAP_PROPERTIES));
		Props.Type = D3D12_HEAP_TYPE_DEFAULT;
		Props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		Props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC Desc;
		HGenericMemory::MemZero(&Desc, 0, sizeof(Desc));
		Desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		Desc.Alignment = 0;
		Desc.Width = Width;
		Desc.Height = Height;
		Desc.DepthOrArraySize = 1;
		Desc.MipLevels = 1;
		Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		Desc.SampleDesc.Count = 1;
		Desc.SampleDesc.Quality = 0;
		Desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		Desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ID3D12Resource* Texture = nullptr;
		DeviceObject->CreateCommittedResource(&Props, D3D12_HEAP_FLAG_NONE, &Desc,
			D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&Texture));

		uint32 UploadPitch = (Width * 4 + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);
		uint32 UploadSize = Height * UploadPitch;
		Desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		Desc.Alignment = 0;
		Desc.Width = UploadSize;
		Desc.Height = 1;
		Desc.DepthOrArraySize = 1;
		Desc.MipLevels = 1;
		Desc.Format = DXGI_FORMAT_UNKNOWN;
		Desc.SampleDesc.Count = 1;
		Desc.SampleDesc.Quality = 0;
		Desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		Desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		Props.Type = D3D12_HEAP_TYPE_UPLOAD;
		Props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		Props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		ID3D12Resource* UploadBuffer = nullptr;
		HRESULT Hr = DeviceObject->CreateCommittedResource(&Props, D3D12_HEAP_FLAG_NONE, &Desc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&UploadBuffer));
		check(SUCCEEDED(Hr));

		void* Mapped = nullptr;
		D3D12_RANGE Range = { 0, UploadSize };
		Hr = UploadBuffer->Map(0, &Range, &Mapped);
		check(SUCCEEDED(Hr));

		for (int32 Y = 0; Y < Height; ++Y)
			HGenericMemory::MemCopy((void*)((uint8*)Mapped + Y * UploadPitch), Pixels + Y * Width * 4, Width * 4);
		UploadBuffer->Unmap(0, &Range);

		D3D12_TEXTURE_COPY_LOCATION SrvLocation = {};
		SrvLocation.pResource = UploadBuffer;
		SrvLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		SrvLocation.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SrvLocation.PlacedFootprint.Footprint.Width = Width;
		SrvLocation.PlacedFootprint.Footprint.Height = Height;
		SrvLocation.PlacedFootprint.Footprint.Depth = 1;
		SrvLocation.PlacedFootprint.Footprint.RowPitch = UploadPitch;

		D3D12_TEXTURE_COPY_LOCATION DstLocation = {};
		DstLocation.pResource = Texture;
		DstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		DstLocation.SubresourceIndex = 0;

		D3D12_RESOURCE_BARRIER Barrier = {};
		Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		Barrier.Transition.pResource = Texture;
		Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

		ID3D12Fence* Fence = nullptr;
		Hr = DeviceObject->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence));
		check(SUCCEEDED(Hr));

		HANDLE Event = CreateEvent(0, 0, 0, 0);
		check(Event != nullptr);

		D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
		QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		QueueDesc.NodeMask = 1;

		ID3D12CommandQueue* CmdQueue = nullptr;
		Hr = DeviceObject->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&CmdQueue));
		check(SUCCEEDED(Hr));

		ID3D12CommandAllocator* CmdAlloc = nullptr;
		Hr = DeviceObject->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CmdAlloc));
		check(SUCCEEDED(Hr));

		ID3D12GraphicsCommandList* CmdList = nullptr;
		Hr = DeviceObject->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CmdAlloc, nullptr, IID_PPV_ARGS(&CmdList));
		check(SUCCEEDED(Hr));

		CmdList->CopyTextureRegion(&DstLocation, 0, 0, 0, &SrvLocation, nullptr);
		CmdList->ResourceBarrier(1, &Barrier);

		Hr = CmdList->Close();
		check(SUCCEEDED(Hr));

		CmdQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&CmdList);
		Hr = CmdQueue->Signal(Fence, 1);
		check(SUCCEEDED(Hr));

		Fence->SetEventOnCompletion(1, Event);
		WaitForSingleObject(Event, INFINITE);

		CmdList->Release();
		CmdAlloc->Release();
		CmdQueue->Release();
		CloseHandle(Event);
		Fence->Release();
		UploadBuffer->Release();

		// create texture view
		D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc;
		HGenericMemory::MemZero(&SrvDesc, 0, sizeof(SrvDesc));
		SrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SrvDesc.Texture2D.MipLevels = Desc.MipLevels;
		SrvDesc.Texture2D.MostDetailedMip = 0;
		SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		DeviceObject->CreateShaderResourceView(Texture,
			&SrvDesc, FontSrvCpuDescHandle);
		SAFE_RELEASE(FontTextureResource);
		FontTextureResource = Texture;
	}

	// store our identifier
	static_assert(sizeof(ImTextureID) >= sizeof(FontSrvGpuDescHandle.ptr), "Can't pack descriptor handle into TexID, 32-bit not supported yet.");
	IO.Fonts->TexID = (ImTextureID)FontSrvGpuDescHandle.ptr;
}

void HImGuiRenderDeviceDirectX12::DestroyDeviceObjects()
{
	ID3D12Device* DeviceObject = (ID3D12Device*)GDynamicRHI->GetDevice();
	if (DeviceObject)
	{
		return;
	}

	SAFE_RELEASE(VertexShaderBlob);
	SAFE_RELEASE(PixelShaderBlob);
	SAFE_RELEASE(D3D12RootSignature);
	SAFE_RELEASE(D3D12PipelineState);
	SAFE_RELEASE(FontTextureResource);

	for (uint32 Index = 0; Index < NUM_FRAMES_INFLIGHT; ++Index)
	{
		SAFE_RELEASE(FrameResources[Index].IB);
		SAFE_RELEASE(FrameResources[Index].VB);
	}
}

bool HImGuiRenderDeviceDirectX12::CreateDeviceObjects()
{
	ID3D12Device* DeviceObject = (ID3D12Device*)GDynamicRHI->GetDevice();
	if (DeviceObject == nullptr)
	{
		return false;
	}

	if (D3D12PipelineState)
	{
		DestroyDeviceObjects();
	}

	// create the root signature
	{
		D3D12_DESCRIPTOR_RANGE DescRange = {};
		DescRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		DescRange.NumDescriptors = 1;
		DescRange.BaseShaderRegister = 0;
		DescRange.RegisterSpace = 0;
		DescRange.OffsetInDescriptorsFromTableStart = 0;

		D3D12_ROOT_PARAMETER Params[2] = {};
		Params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		Params[0].Constants.ShaderRegister = 0;
		Params[0].Constants.RegisterSpace = 0;
		Params[0].Constants.Num32BitValues = 16;
		Params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

		Params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		Params[1].DescriptorTable.NumDescriptorRanges = 1;
		Params[1].DescriptorTable.pDescriptorRanges = &DescRange;
		Params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_STATIC_SAMPLER_DESC StaticSampler = {};
		StaticSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		StaticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		StaticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		StaticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		StaticSampler.MipLODBias = 0.f;
		StaticSampler.MaxAnisotropy = 0;
		StaticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		StaticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		StaticSampler.MinLOD = 0.f;
		StaticSampler.MaxLOD = 0.f;
		StaticSampler.ShaderRegister = 0;
		StaticSampler.RegisterSpace = 0;
		StaticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_ROOT_SIGNATURE_DESC Desc = {};
		Desc.NumParameters = _countof(Params);
		Desc.pParameters = Params;
		Desc.NumStaticSamplers = 1;
		Desc.pStaticSamplers = &StaticSampler;
		Desc.Flags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		ID3DBlob* Blob = nullptr;
		if (D3D12SerializeRootSignature(&Desc, D3D_ROOT_SIGNATURE_VERSION_1, &Blob, nullptr) != S_OK)
		{
			return false;
		}

		DeviceObject->CreateRootSignature(0, Blob->GetBufferPointer(), Blob->GetBufferSize(), IID_PPV_ARGS(&D3D12RootSignature));
		SAFE_RELEASE(Blob);
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC PSODesc;
	HGenericMemory::MemZero(&PSODesc, 0, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PSODesc.NodeMask = 1;
	PSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PSODesc.pRootSignature = D3D12RootSignature;
	PSODesc.SampleMask = UINT_MAX;
	PSODesc.NumRenderTargets = 1;
	PSODesc.RTVFormats[0] = RTVFormat;
	PSODesc.SampleDesc.Count = 1;
	PSODesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	// create the input layout
	static D3D12_INPUT_ELEMENT_DESC LocalLayout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->pos), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->uv),  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (size_t)(&((ImDrawVert*)0)->col), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
	PSODesc.InputLayout = { LocalLayout, 3 };

	// vertex shader
	PSODesc.VS = {
		GImGuiVertexShader->SerializedResource->Code.data(),
		GImGuiVertexShader->SerializedResource->Code.size()
	};

	// pixel shader
	PSODesc.PS = {
		GImGuiPixelShader->SerializedResource->Code.data(),
		GImGuiPixelShader->SerializedResource->Code.size()
	};

	// Create the blending setup
	{
		D3D12_BLEND_DESC& desc = PSODesc.BlendState;
		desc.AlphaToCoverageEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}

	// Create the rasterizer state
	{
		D3D12_RASTERIZER_DESC& desc = PSODesc.RasterizerState;
		desc.FillMode = D3D12_FILL_MODE_SOLID;
		desc.CullMode = D3D12_CULL_MODE_NONE;
		desc.FrontCounterClockwise = FALSE;
		desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		desc.DepthClipEnable = true;
		desc.MultisampleEnable = FALSE;
		desc.AntialiasedLineEnable = FALSE;
		desc.ForcedSampleCount = 0;
		desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	}

	// Create depth-stencil State
	{
		D3D12_DEPTH_STENCIL_DESC& desc = PSODesc.DepthStencilState;
		desc.DepthEnable = false;
		desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		desc.StencilEnable = false;
		desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		desc.BackFace = desc.FrontFace;
	}

	if (DeviceObject->CreateGraphicsPipelineState(&PSODesc, IID_PPV_ARGS(&D3D12PipelineState)) != S_OK)
	{
		return false;
	}

	CreateFontsTexture();

	return true;
}

void HImGuiRenderDeviceDirectX12::Tick()
{
	if (D3D12PipelineState == nullptr)
	{
		CreateDeviceObjects();
	}
}

void HImGuiRenderDeviceDirectX12::RenderDrawData(uint32 InFrameIndex, ImDrawData* InDrawData, ID3D12GraphicsCommandList* InCommandList)
{
	ID3D12Device* DeviceObject = (ID3D12Device*)GDynamicRHI->GetDevice();

	// @todo - I'm assuming that this only gets called once per frame
	// if not, we can't just re-allocate the IB or VB, we'll have to do a proper allocator
	HFrameResources& FrameResource = FrameResources[InFrameIndex % NUM_FRAMES_INFLIGHT];
	ID3D12Resource* FrameVB = FrameResource.VB;
	ID3D12Resource* FrameIB = FrameResource.IB;
	int32 VertexBufferSize = FrameResource.VertexBufferSize;
	int32 IndexBufferSize = FrameResource.IndexBufferSize;

	// create and grow vertex/index buffers if needed
	if (!FrameVB || VertexBufferSize < InDrawData->TotalVtxCount)
	{
		SAFE_RELEASE(FrameVB);

		VertexBufferSize = InDrawData->TotalVtxCount + 5000;

		D3D12_HEAP_PROPERTIES Props;
		HGenericMemory::MemZero(&Props, 0, sizeof(Props));
		Props.Type = D3D12_HEAP_TYPE_UPLOAD;
		Props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		Props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC Desc;
		HGenericMemory::MemZero(&Desc, 0, sizeof(Desc));
		Desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		Desc.Width = VertexBufferSize * sizeof(ImDrawVert);
		Desc.Height = 1;
		Desc.DepthOrArraySize = 1;
		Desc.MipLevels = 1;
		Desc.Format = DXGI_FORMAT_UNKNOWN;
		Desc.SampleDesc.Count = 1;
		Desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		Desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		if (DeviceObject->CreateCommittedResource(&Props, D3D12_HEAP_FLAG_NONE, &Desc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&FrameVB)) < 0)
		{
			return;
		}

		FrameResource.VB = FrameVB;
		FrameResource.VertexBufferSize = VertexBufferSize;
	}
	if (!FrameIB || IndexBufferSize < InDrawData->TotalIdxCount)
	{
		SAFE_RELEASE(FrameIB);

		IndexBufferSize = InDrawData->TotalIdxCount + 10000;

		D3D12_HEAP_PROPERTIES Props;
		memset(&Props, 0, sizeof(D3D12_HEAP_PROPERTIES));
		Props.Type = D3D12_HEAP_TYPE_UPLOAD;
		Props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		Props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC Desc;
		memset(&Desc, 0, sizeof(D3D12_RESOURCE_DESC));
		Desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		Desc.Width = IndexBufferSize * sizeof(ImDrawIdx);
		Desc.Height = 1;
		Desc.DepthOrArraySize = 1;
		Desc.MipLevels = 1;
		Desc.Format = DXGI_FORMAT_UNKNOWN;
		Desc.SampleDesc.Count = 1;
		Desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		Desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		if (DeviceObject->CreateCommittedResource(&Props, D3D12_HEAP_FLAG_NONE, &Desc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&FrameIB)) < 0)
			return;

		FrameResource.IB = FrameIB;
		FrameResource.IndexBufferSize = IndexBufferSize;
	}

	// copy and convert all vertices into a single contiguous buffer
	void* VtxResource, *IdxResource;

	D3D12_RANGE Range;
	HGenericMemory::MemZero(&Range, 0, sizeof(D3D12_RANGE));

	if (FrameVB->Map(0, &Range, &VtxResource) != S_OK)
	{
		return;
	}
	if (FrameIB->Map(0, &Range, &IdxResource) != S_OK)
	{
		return;
	}

	ImDrawVert* VtxDst = (ImDrawVert*)VtxResource;
	ImDrawIdx* IdxDst = (ImDrawIdx*)IdxResource;
	for (int32 Index = 0; Index < InDrawData->CmdListsCount; ++Index)
	{
		const ImDrawList* CmdList = InDrawData->CmdLists[Index];
		HGenericMemory::MemCopy(VtxDst, CmdList->VtxBuffer.Data, CmdList->VtxBuffer.Size * sizeof(ImDrawVert));
		HGenericMemory::MemCopy(IdxDst, CmdList->IdxBuffer.Data, CmdList->IdxBuffer.Size * sizeof(ImDrawIdx));

		VtxDst += CmdList->VtxBuffer.Size;
		IdxDst += CmdList->IdxBuffer.Size;
	}

	FrameVB->Unmap(0, &Range);
	FrameIB->Unmap(0, &Range);

	// setup orthographic projection matrix into our constant buffer
	// our visible imgui space lies from InDrawData->DisplayPos (top left) to InDrawData->DisplayPos + InDrawData->DisplaySize (bottom right)
	HVertexConstantBuffer VertexConstantBuffer;
	{
		HVertexConstantBuffer* ConstantBuffer = &VertexConstantBuffer;
		float L = InDrawData->DisplayPos.x;
		float R = InDrawData->DisplayPos.x + InDrawData->DisplaySize.x;
		float T = InDrawData->DisplayPos.y;
		float B = InDrawData->DisplayPos.y + InDrawData->DisplaySize.y;
		float mvp[4][4] =
		{
			{ 2.0f / (R - L),   0.0f,           0.0f,       0.0f },
			{ 0.0f,         2.0f / (T - B),     0.0f,       0.0f },
			{ 0.0f,         0.0f,           0.5f,       0.0f },
			{ (R + L) / (L - R),  (T + B) / (B - T),    0.5f,       1.0f },
		};
		HGenericMemory::MemCopy(&ConstantBuffer->MVP, mvp, sizeof(mvp));
	}

	// setup viewport
	D3D12_VIEWPORT VP;
	memset(&VP, 0, sizeof(D3D12_VIEWPORT));
	VP.Width = InDrawData->DisplaySize.x;
	VP.Height = InDrawData->DisplaySize.y;
	VP.MinDepth = 0.0f;
	VP.MaxDepth = 1.0f;
	VP.TopLeftX = VP.TopLeftY = 0.0f;

	InCommandList->RSSetViewports(1, &VP);

	// Bind shader and vertex buffers
	uint32 Stride = sizeof(ImDrawVert);
	uint32 Offset = 0;

	D3D12_VERTEX_BUFFER_VIEW VBV;
	memset(&VBV, 0, sizeof(D3D12_VERTEX_BUFFER_VIEW));
	VBV.BufferLocation = FrameVB->GetGPUVirtualAddress() + Offset;
	VBV.SizeInBytes = VertexBufferSize * Stride;
	VBV.StrideInBytes = Stride;
	InCommandList->IASetVertexBuffers(0, 1, &VBV);

	D3D12_INDEX_BUFFER_VIEW IBV;
	memset(&IBV, 0, sizeof(D3D12_INDEX_BUFFER_VIEW));
	IBV.BufferLocation = FrameIB->GetGPUVirtualAddress();
	IBV.SizeInBytes = IndexBufferSize * sizeof(ImDrawIdx);
	IBV.Format = sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

	InCommandList->IASetIndexBuffer(&IBV);
	InCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	InCommandList->SetPipelineState(D3D12PipelineState);
	InCommandList->SetGraphicsRootSignature(D3D12RootSignature);
	InCommandList->SetGraphicsRoot32BitConstants(0, 16, &VertexConstantBuffer, 0);

	// Setup render state
	const float BlendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	InCommandList->OMSetBlendFactor(BlendFactor);

	// Render command lists
	int VtxOffset = 0;
	int IdxOffset = 0;
	ImVec2 Pos = InDrawData->DisplayPos;
	for (int32 Index = 0; Index < InDrawData->CmdListsCount; Index++)
	{
		const ImDrawList* DrawList = InDrawData->CmdLists[Index];
		for (int cmd_i = 0; cmd_i < DrawList->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &DrawList->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(DrawList, pcmd);
			}
			else
			{
				const D3D12_RECT r = { (LONG)(pcmd->ClipRect.x - Pos.x), (LONG)(pcmd->ClipRect.y - Pos.y), (LONG)(pcmd->ClipRect.z - Pos.x), (LONG)(pcmd->ClipRect.w - Pos.y) };
				InCommandList->SetGraphicsRootDescriptorTable(1, *(D3D12_GPU_DESCRIPTOR_HANDLE*)&pcmd->TextureId);
				InCommandList->RSSetScissorRects(1, &r);
				InCommandList->DrawIndexedInstanced(pcmd->ElemCount, 1, IdxOffset, VtxOffset, 0);
			}
			IdxOffset += pcmd->ElemCount;
		}
		VtxOffset += DrawList->VtxBuffer.Size;
	}
}

void HImGuiRenderDeviceDirectX12::Render()
{
	ImGui::Render();
	RenderDrawData(GDynamicRHI->GetFrameIndex(), ImGui::GetDrawData(), (ID3D12GraphicsCommandList*)GDynamicRHI->GetCommandList());
}