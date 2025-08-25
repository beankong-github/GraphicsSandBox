#pragma once
#include "engine.h"

class IDXGIFactory4;
//class IDXGISwapChain3;

#include <dxgi1_4.h>

class Engine_API Device
{
	friend class SBEngine;
	friend struct std::default_delete<Device>; // unique_ptr 삭제 허용

public:
	~Device();

	ID3D12Device* GetDevice() const { return mDevice; }
	ID3D12CommandQueue* GetCommandQueue() const { 	return mCommandQueue; }
	ID3D12CommandList* GetCommandList() const { return mCommandList;	}
private:
	// SBEngine이외에서 mDevice 생성을 막는다.
	Device() = default;
	
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
	void FlushCommandQueue();	// Command queue의 명령 비우기

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

	// TMP
	void StartWriteCommandList();
	void FinishWriteCommandListAndFlush();


	inline ID3D12Resource* CurrentBackBuffer() const;
	inline D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	inline D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

	ID3D12Device*				mDevice;			
	IDXGIFactory4*				mFactory;	
	IDXGISwapChain3*		mSwapChain;

	// Command
	// Command List(CPU) -> Allocatior <- Command Queue(GPU)
	ID3D12CommandQueue* mCommandQueue;
	ID3D12CommandAllocator* mDirectCmdListAlloc;
	ID3D12GraphicsCommandList* mCommandList;

	ID3D12Fence* mFence;
	UINT64 mCurrentFence = 0;

	// Swap Chain
	static const int SwapChainBufferCount = 2;
	int mCurrentBackBuffer = 0;
	ID3D12Resource* mSwapChainBuffer[SwapChainBufferCount];
	ID3D12Resource* mDepthStencilBuffer;

	// 4X MSAA
	// => 스왑체인에서 MSAA는 사용하지 않는다.
	bool      m4xMsaaState = false;    // 4X MSAA enabled
	UINT      m4xMsaaQuality = 0;      // quality level of 4X MSAA
	
	// Descriptor
	ID3D12DescriptorHeap* mRtvHeap;
	ID3D12DescriptorHeap* mDsvHeap;

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