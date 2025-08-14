#pragma once
#include "engine.h"

class IDXGIFactory4;
class IDXGISwapChain3;

class Device
{
	friend class SBEngine;

private:
	// SBEngine이외에서 Device 생성을 막는다.
	Device();

	bool Init();
	bool InitDirect3D();
	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateRTVAndDSVDescriptorHeaps();

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

	ComPtr<ID3D12Device>				mDevice;			
	ComPtr<IDXGIFactory4>				mFactory;	
	ComPtr<IDXGISwapChain3>		mSwapChain;

	// Command
	// Command List(CPU) -> Allocatior <- Command Queue(GPU)
	ComPtr<ID3D12CommandQueue> mCommandQueue;
	ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
	ComPtr<ID3D12GraphicsCommandList> mCommandList;

	ComPtr<ID3D12Fence> mFence;
	UINT64 mCurrentFence = 0;

	// Swap Chain
	static const int SwapChainBufferCount = 2;
	int mCurrBackBuffer = 0;
	ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	ComPtr<ID3D12Resource> mDepthStencilBuffer;

	// 4X MSAA
	// Set true to use 4X MSAA (§4.1.8).  The default is false.
	// TODO  :: 나중에 MASS 켜기
	bool      m4xMsaaState = false;    // 4X MSAA enabled
	UINT      m4xMsaaQuality = 0;      // quality level of 4X MSAA
	
	// Descriptor
	ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	ComPtr<ID3D12DescriptorHeap> mDsvHeap;

	//화면
	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;

	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;


	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
};