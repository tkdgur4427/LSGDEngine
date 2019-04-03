#include "HEnginePCH.h"
#include "HDynamicRHI.h"

using namespace lsgd;

// @todo - just temporary! need to refactoring when I try to follow UE4 design
//{

#define NUM_FRAMES_IN_FLIGHT 3
struct HFrameContext
{
	ID3D12CommandAllocator* CommandAllocator;
	uint64 FenceValue;
};
HFrameContext D3D12FrameContexts[NUM_FRAMES_IN_FLIGHT] = {};
HFrameContext* CurrentFrameContext = nullptr;
uint32 FrameIndex = 0;

#define NUM_BACK_BUFFERS 3
#define DX12_ENABLE_DEBUG_LAYER 1

static ID3D12Device* D3D12Device = nullptr;
static ID3D12DescriptorHeap* D3D12RtvDescriptorHeap = nullptr;
static ID3D12DescriptorHeap* D3D12SrvDescriptorHeap = nullptr;

static ID3D12Resource* D3D12MainRenderTargetResource[NUM_BACK_BUFFERS] = { 0, };
static D3D12_CPU_DESCRIPTOR_HANDLE D3D12MainRenderTargetDescriptors[NUM_BACK_BUFFERS] = {};

static ID3D12CommandQueue* D3D12CommandQueue = nullptr;
static ID3D12GraphicsCommandList* D3D12CommandList = nullptr;
static ID3D12Fence* D3D12Fence = nullptr;
static HANDLE D3D12FenceEvent = nullptr;

static IDXGISwapChain3* D3D12SwapChain = nullptr;
static HANDLE D3D12SwapChainWaitableObject = nullptr;

static uint64 D3D12FenceLastSignaledValue = 0;

unique_ptr<lsgd::HDynamicRHI> GDynamicRHI;

HFrameContext* WaitForNextFrameResources()
{
	uint32 NextFrameIndex = FrameIndex + 1;
	FrameIndex = NextFrameIndex;

	HANDLE WaitableObjects[] = { D3D12SwapChainWaitableObject, nullptr };
	int32 NumWaitableObjects = 1;

	HFrameContext& FrameContext = D3D12FrameContexts[NextFrameIndex % NUM_FRAMES_IN_FLIGHT];
	uint64 FenceValue = FrameContext.FenceValue;
	if (FenceValue != 0) // means no fence was signaled
	{
		FrameContext.FenceValue = 0;
		D3D12Fence->SetEventOnCompletion(FenceValue, D3D12FenceEvent);
		WaitableObjects[1] = D3D12FenceEvent;
		NumWaitableObjects = 2;
	}

	WaitForMultipleObjects(NumWaitableObjects, WaitableObjects, true, INFINITE);

	return &FrameContext;
}

//}

void* HDynamicRHI::GetDevice()
{
	check(D3D12Device);
	return (void*)D3D12Device;
}

void* HDynamicRHI::GetSrvDescriptionHeap()
{
	check(D3D12SrvDescriptorHeap);
	return (void*)D3D12SrvDescriptorHeap;
}

bool HDynamicRHI::CreateDevice(const HWindowFrame& InWindowFrame)
{
	// setup swap chain
	HWND Handle = (HWND)InWindowFrame.GetHandle();

	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC1 sd;
	{
		HGenericMemory::MemZero(&sd, 0, sizeof(sd));
		sd.BufferCount = NUM_BACK_BUFFERS;
		sd.Width = 0;
		sd.Height = 0;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		sd.Scaling = DXGI_SCALING_STRETCH;
		sd.Stereo = FALSE;
	}

	if (DX12_ENABLE_DEBUG_LAYER)
	{
		ID3D12Debug* dx12Debug = NULL;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&dx12Debug))))
		{
			dx12Debug->EnableDebugLayer();
			dx12Debug->Release();
		}
	}

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	if (D3D12CreateDevice(NULL, featureLevel, IID_PPV_ARGS(&D3D12Device)) != S_OK)
		return false;

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = NUM_BACK_BUFFERS;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 1;
		if (D3D12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&D3D12RtvDescriptorHeap)) != S_OK)
			return E_FAIL;

		SIZE_T rtvDescriptorSize = D3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = D3D12RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
		{
			D3D12MainRenderTargetDescriptors[i] = rtvHandle;
			rtvHandle.ptr += rtvDescriptorSize;
		}
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (D3D12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&D3D12SrvDescriptorHeap)) != S_OK)
			return E_FAIL;
	}

	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 1;
		if (D3D12Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&D3D12CommandQueue)) != S_OK)
			return E_FAIL;
	}

	for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
		if (D3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&D3D12FrameContexts[i].CommandAllocator)) != S_OK)
			return E_FAIL;

	if (D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12FrameContexts[0].CommandAllocator, NULL, IID_PPV_ARGS(&D3D12CommandList)) != S_OK ||
		D3D12CommandList->Close() != S_OK)
		return E_FAIL;

	if (D3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&D3D12Fence)) != S_OK)
		return E_FAIL;

	D3D12FenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (D3D12FenceEvent == NULL)
		return false;

	{
		IDXGIFactory4* dxgiFactory = NULL;
		IDXGISwapChain1* swapChain1 = NULL;
		if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK ||
			dxgiFactory->CreateSwapChainForHwnd(D3D12CommandQueue, Handle, &sd, NULL, NULL, &swapChain1) != S_OK ||
			swapChain1->QueryInterface(IID_PPV_ARGS(&D3D12SwapChain)) != S_OK)
			return E_FAIL;
		swapChain1->Release();
		dxgiFactory->Release();
		D3D12SwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
		D3D12SwapChainWaitableObject = D3D12SwapChain->GetFrameLatencyWaitableObject();
	}

	ID3D12Resource* BackBuffer;
	for (uint32 Index = 0; Index < NUM_BACK_BUFFERS; ++Index)
	{
		D3D12SwapChain->GetBuffer(Index, IID_PPV_ARGS(&BackBuffer));
		D3D12Device->CreateRenderTargetView(BackBuffer, NULL, D3D12MainRenderTargetDescriptors[Index]);
		D3D12MainRenderTargetResource[Index] = BackBuffer;
	}

	return true;
}

void HDynamicRHI::WaitForLastSubmittedFrame()
{
	HFrameContext& FrameContext = D3D12FrameContexts[FrameIndex % NUM_FRAMES_IN_FLIGHT];

	uint64 FenceValue = FrameContext.FenceValue;
	if (FenceValue == 0)
		return; // no fence was signaled

	FrameContext.FenceValue = 0;
	if (D3D12Fence->GetCompletedValue() >= FenceValue)
		return;

	D3D12Fence->SetEventOnCompletion(FenceValue, D3D12FenceEvent);
	WaitForSingleObject(D3D12FenceEvent, INFINITE);
}

void HDynamicRHI::CleanupRenderTarget()
{
	WaitForLastSubmittedFrame();

	for (uint32 Index = 0; Index < NUM_BACK_BUFFERS; ++Index)
	{
		if (D3D12MainRenderTargetResource[Index])
		{
			D3D12MainRenderTargetResource[Index]->Release();
		}
	}
}

void HDynamicRHI::CleanupDevice()
{
	CleanupRenderTarget();
	if (D3D12SwapChain)
	{
		D3D12SwapChain->Release();
		D3D12SwapChain = nullptr;
	}
	if (D3D12SwapChainWaitableObject != nullptr)
	{
		CloseHandle(D3D12SwapChainWaitableObject);
		D3D12SwapChainWaitableObject = nullptr;
	}

	for (uint32 Index = 0; Index < NUM_FRAMES_IN_FLIGHT; ++Index)
	{
		if (D3D12FrameContexts[Index].CommandAllocator)
		{
			D3D12FrameContexts[Index].CommandAllocator->Release();
			D3D12FrameContexts[Index].CommandAllocator = nullptr;
		}
	}

	SAFE_RELEASE(D3D12CommandQueue);
	SAFE_RELEASE(D3D12CommandList);
	SAFE_RELEASE(D3D12RtvDescriptorHeap);
	SAFE_RELEASE(D3D12SrvDescriptorHeap);
	SAFE_RELEASE(D3D12Fence);
	SAFE_RELEASE(D3D12Device);

	if (D3D12FenceEvent != nullptr)
	{
		CloseHandle(D3D12FenceEvent);
		D3D12FenceEvent = nullptr;
	}
}

void HDynamicRHI::RenderBegin()
{
	ImVec4 ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	HFrameContext* FrameContext = WaitForNextFrameResources();
	
	check(CurrentFrameContext == nullptr);
	CurrentFrameContext = FrameContext;

	uint32 BackBufferIndex = D3D12SwapChain->GetCurrentBackBufferIndex();
	FrameContext->CommandAllocator->Reset();

	D3D12_RESOURCE_BARRIER Barrier = {};
	Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	Barrier.Transition.pResource = D3D12MainRenderTargetResource[BackBufferIndex];
	Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	D3D12CommandList->Reset(FrameContext->CommandAllocator, nullptr);
	D3D12CommandList->ResourceBarrier(1, &Barrier);
	D3D12CommandList->ClearRenderTargetView(D3D12MainRenderTargetDescriptors[BackBufferIndex], (float*)&ClearColor, 0, nullptr);
	D3D12CommandList->OMSetRenderTargets(1, &D3D12MainRenderTargetDescriptors[BackBufferIndex], false, nullptr);
	D3D12CommandList->SetDescriptorHeaps(1, &D3D12SrvDescriptorHeap);
}

void HDynamicRHI::RenderEnd()
{
	uint32 BackBufferIndex = D3D12SwapChain->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER Barrier = {};
	Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	Barrier.Transition.pResource = D3D12MainRenderTargetResource[BackBufferIndex];
	Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	D3D12CommandList->ResourceBarrier(1, &Barrier);

	D3D12CommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&D3D12CommandList);

	D3D12SwapChain->Present(1, 0);

	UINT64 FenceValue = D3D12FenceLastSignaledValue + 1;
	D3D12CommandQueue->Signal(D3D12Fence, FenceValue);
	D3D12FenceLastSignaledValue = FenceValue;

	check(CurrentFrameContext);
	CurrentFrameContext->FenceValue = FenceValue;
	CurrentFrameContext = nullptr;
}

void* HDynamicRHI::GetCommandList()
{
	return D3D12CommandList;
}

uint32 HDynamicRHI::GetFrameIndex() const
{
	return FrameIndex;
}

void HDynamicRHI::CreateVertexShader(const HArray<uint8>& InCode)
{

}

void HDynamicRHI::CreatePixelShader(const HArray<uint8>& InCode)
{

}