#pragma once
#include "engine.h"

class IDXGIFactory4;
class IDXGISwapChain3;

class Engine_API Device
{
	friend class SBEngine;
	friend struct std::default_delete<Device>; // unique_ptr ���� ���

private:
	// SBEngine�̿ܿ��� mDevice ������ ���´�.
	Device() = default;
	~Device() = default;


	Device(const Device&) = delete;
	Device& operator=(const Device&) = delete;
	Device(Device&&) = delete;
	Device& operator=(Device&&) = delete;

	bool Init();
	bool InitDirect3D();
	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateRTVAndDSVDescriptorHeaps();

	void Update();
	void Draw();

	void OnResizeWindow();
	void FlushCommandQueue();	// Command queue�� ��� ����

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

	inline ID3D12Resource* CurrentBackBuffer() const;
	inline D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	inline D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;


	ComPtr<ID3D12Device>		mDevice;			
	ComPtr<IDXGIFactory4>		mFactory;	
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
	int mCurrentBackBuffer = 0;
	ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	ComPtr<ID3D12Resource> mDepthStencilBuffer;

	// 4X MSAA
	// => ����ü�ο��� MSAA�� ������� �ʴ´�.
	bool      m4xMsaaState = false;    // 4X MSAA enabled
	UINT      m4xMsaaQuality = 0;      // quality level of 4X MSAA
	
	// Descriptor
	ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	ComPtr<ID3D12DescriptorHeap> mDsvHeap;

	//ȭ��
	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;

	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;


	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
};