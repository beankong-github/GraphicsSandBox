#include "pch.h"
#include "Device.h"
#include "SBEngine.h"

bool Device::Init()
{
	if (InitDirect3D())
	{
		OnResizeWindow();
		return true;
	}

	return false;
}

bool Device::InitDirect3D()
{
	UINT factoryFlags = 0;
#if defined(DEBUG) || defined(_DEBUG) 
	{
		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	ThrowIfFailed(CreateDXGIFactory2(factoryFlags,IID_PPV_ARGS(&mFactory)));
	
	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,             // default adapter
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&mDevice));
	
	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(mFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
	
		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&mDevice)));
	}
	
	// Fence ����
	ThrowIfFailed(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&mFence)));
	
	// Descriptor ������ ��������
	mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	// ��Ƽ ���ø� �ʱ�ȭ
	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = mBackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(mDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));
	
	m4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

#ifdef _DEBUG
	LogAdapters();
#endif

	// CPU->GPU ��� Command Object ����
	CreateCommandObjects();
	// ���� ü�� ����
	CreateSwapChain();
	// Render Target View �� Depth Stencil View ������ �� ����
	CreateRTVAndDSVDescriptorHeaps();

	return true;
}

void Device::CreateCommandObjects()
{
	// Command Queue
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));
	
	// Command Allocator
	ThrowIfFailed(mDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(mDirectCmdListAlloc.GetAddressOf())));

	// CommandList
	ThrowIfFailed(mDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		mDirectCmdListAlloc.Get(), // Associated command allocator
		nullptr,                   // Initial PipelineStateObject
		IID_PPV_ARGS(mCommandList.GetAddressOf())));

	// Command List�� ���� ���·� ���۵Ǿ�� �մϴ�.
	// �ֳĸ� ó�� Command List�� ������ ����� �ۼ��ϱ� ���� Reset()�� ȣ���ϴµ�
	// Reset�� ȣ���ϱ� ���� Command List�� �ݵ�� �����־�� �ϱ� �����̴�.
	mCommandList->Close();
}

void Device::CreateSwapChain()
{
	Vector2 resolution = SBEngine::Get()->GetResolution();
	HWND windowHandle = SBEngine::Get()->GetMainHwnd();

	// Release the previous swapchain we will be recreating.
	mSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC1 sd;
	sd.Width = resolution.x;
	sd.Height = resolution.y;
	sd.Format = mBackBufferFormat;
	sd.Stereo = false; 
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.Scaling = DXGI_SCALING_NONE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	sd.Flags = 0;

	ComPtr<IDXGISwapChain1> sc1;
	ThrowIfFailed(mFactory->CreateSwapChainForHwnd(
		mCommandQueue.Get(),	// command queue
		SBEngine::Get()->GetMainHwnd(),
		&sd,
		nullptr,
		nullptr,
		sc1.ReleaseAndGetAddressOf()));

	
	ThrowIfFailed(sc1.As(&mSwapChain));

}

void Device::CreateRTVAndDSVDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;

	ThrowIfFailed(mDevice->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));


	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(mDevice->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}

void Device::Update()
{
}

void Device::Draw()
{
	// Command List������ ���� mDirectCmdListAlloc Reset
	ThrowIfFailed(mDirectCmdListAlloc->Reset());
	// Command List ������ ���� Reset
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	// BackBuffer�� Resource Barrier ����
	// Resource Barrier?
	// ���ҽ��� ���� ������ ���� ��ü
	// ���� Ÿ���� Clear�ϱ� ���� ���¸� STATE_RENDER_TARGET�� �������ش�.
	D3D12_RESOURCE_BARRIER barrier;
	ZeroMemory(&barrier, sizeof(barrier));
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAGS::D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = CurrentBackBuffer();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	mCommandList->ResourceBarrier(1, &barrier);

	// ViewPort ���� ����
	mCommandList->RSSetViewports(1, &mScreenViewport);
	// Scissor Rect ����
	// ���� ���簢��(Scissor Rectangle)�� Ư�� �ȼ����� ����(Culling)�ϴ� �뵵�� ���δ�. �ĸ� ���۸� �������� ���� ���簢���� ����, �����ϸ�, ������ �� ���� ���簢���� �ٱ��� �ִ� �ȼ����� �ĸ� ���ۿ� ������ȭ ���� �ʴ´�. �̷��� �ȼ� ������ ������ ����ȭ ����̴�.
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	// ����ۿ� ���� ���� Ŭ����
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// �������� ���۸� �����Ѵ�.
	D3D12_CPU_DESCRIPTOR_HANDLE bbv = CurrentBackBufferView();
	D3D12_CPU_DESCRIPTOR_HANDLE dsv = DepthStencilView();
	mCommandList->OMSetRenderTargets(1, &bbv, true, &dsv);

	// GPU���� Render Target�� ���� �� �ְ� 
	// BackBuffer�� ���¸� PRESENT���·� �ٲ��ش�.
	ZeroMemory(&barrier, sizeof(barrier));
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAGS::D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = CurrentBackBuffer();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	// Command List �ۼ� ����!
	ThrowIfFailed(mCommandList->Close());

	// Command List�� ����� �����ϱ� ���� Queue�� ������.
	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	// buffer swap! (front <-> back)
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrentBackBuffer = (mCurrentBackBuffer + 1) % SwapChainBufferCount;

	// Command Queue ����� ��� �Ϸ�� ������ ����Ѵ�.
	// TODO :: ������ ����� �Ϸ�� ������ ��ٸ���. �� ����� ��ȿ�����̸� �ܼ�ȭ�� ���� ���� ���̴�. ���Ŀ��� �� �����Ӹ��� ��ٸ��� �ʾƵ� �ǵ��� ������ �ڵ带 �����ϴ� ����� ������ ���̴�.
	FlushCommandQueue();
}

void Device::OnResizeWindow()
{
	assert(mDevice);
	assert(mSwapChain);
	assert(mDirectCmdListAlloc);

	Vector2 newResolution = SBEngine::Get()->GetResolution();

	// Command Queue ����� ��� �Ϸ�� ������ ����Ѵ�.
	FlushCommandQueue();

	// mDirectCmdListAlloc ����
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	// SwapChainBuffer(Front, Back)�� DSBuffer ����
	for (int i = 0; i < SwapChainBufferCount; ++i)
		mSwapChainBuffer[i].Reset();
	mDepthStencilBuffer.Reset();


	// SwapChainBuffer(Front, Back) ��������
	ThrowIfFailed(mSwapChain->ResizeBuffers(
		SwapChainBufferCount,
		newResolution.x, newResolution.y,
		mBackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	// ���� BackBuffer Index 0���� ����(�ʱ�ȭ)
	mCurrentBackBuffer = 0;

	// SwapChainBufferView ����
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));
		mDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(),
										nullptr,
										rtvHeapHandle);
		rtvHeapHandle.ptr += mRtvDescriptorSize;
	}
	
	// depth/stencil buffer�� view ����
	// DSbuffer ����
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = newResolution.x;
	depthStencilDesc.Height = newResolution.y;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
	// the depth buffer.  Therefore, because we need to create two views to the same resource:
	//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
	// we need to create the depth buffer resource with a typeless format.  
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	
	// DS �ʱ�ȭ ����
	D3D12_CLEAR_VALUE optClear;
	optClear.Format = mDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;		// Depth ���� 1�� �ʱ�ȭ (���� �� ��)
	optClear.DepthStencil.Stencil = 0;
	
	D3D12_HEAP_PROPERTIES heap;
	heap.Type = D3D12_HEAP_TYPE_DEFAULT;		// GPU ���� VRAM.
	heap.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heap.CreationNodeMask = 1;
	heap.VisibleNodeMask = 1;

	// DSBuffer ����
	ThrowIfFailed(mDevice->CreateCommittedResource(
		&heap,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&optClear,
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));
	
	// DSV ����
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = mDepthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	mDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, DepthStencilView());

	// ��� resize ����� �����Ͽ� �߰������Ƿ� mCommandList �ݱ�
	ThrowIfFailed(mCommandList->Close());
	// Command Queue�� ��� ����
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Command Queue �Ϸ� ���
	FlushCommandQueue();

	// Ŭ���̾�Ʈ�� ���� viewport transform ������Ʈ
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(newResolution.x);
	mScreenViewport.Height = static_cast<float>(newResolution.x);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;
	
	mScissorRect = { 0, 0, static_cast<long>(newResolution.x), static_cast<long>(newResolution.y) };
}

void Device::FlushCommandQueue()
{
	mCurrentFence++;

	ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mCurrentFence));

	if (mFence->GetCompletedValue() < mCurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, 0, 0, EVENT_ALL_ACCESS);

		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));

		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

}

void Device::LogAdapters()
{
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;
	while (mFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		std::wstring text = L"***Adapter: ";
		text += desc.Description;
		text += L"\n";

		OutputDebugString(text.c_str());

		adapterList.push_back(adapter);

		++i;
	}

	for (size_t i = 0; i < adapterList.size(); ++i)
	{
		LogAdapterOutputs(adapterList[i]);
		ReleaseCom(adapterList[i]);
	}
}

void Device::LogAdapterOutputs(IDXGIAdapter* adapter)
{
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring text = L"***Output: ";
		text += desc.DeviceName;
		text += L"\n";
		OutputDebugString(text.c_str());

		LogOutputDisplayModes(output, mBackBufferFormat);

		ReleaseCom(output);

		++i;
	}
}

void Device::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
{
}

inline ID3D12Resource* Device::CurrentBackBuffer() const
{
	return mSwapChainBuffer[mCurrentBackBuffer].Get();
}

inline D3D12_CPU_DESCRIPTOR_HANDLE Device::CurrentBackBufferView() const
{
	D3D12_CPU_DESCRIPTOR_HANDLE curBackBufferView;
	
	// RTV HEAP ���� �ּ� + Buffer Index *  RTV ũ�� 
	curBackBufferView.ptr = mRtvHeap->GetCPUDescriptorHandleForHeapStart().ptr + mCurrentBackBuffer * mRtvDescriptorSize;
	
	return curBackBufferView;
}

inline D3D12_CPU_DESCRIPTOR_HANDLE Device::DepthStencilView() const
{
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}
