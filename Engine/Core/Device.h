#pragma once
#include "engine.h"

class IDXGIFactory4;
class IDXGISwapChain3;

class Device
{
	friend class SBEngine;

private:
	// SBEngine�̿ܿ��� Device ������ ���´�.
	Device();

	bool Init();

	ComPtr<ID3D12Device>		device;			
	ComPtr<IDXGIFactory>		factory;	// ��� �������¿�
	ComPtr<IDXGISwapChain3>		swapChain;

	bool MSAA4xEnable = { false };
	UINT m_nMsaa4xQualityLevels = { 0 };
	static const UINT m_nSwapChainBuffers = { 2 };
	UINT m_nSwapChainBufferIndex;

	ID3D12Resource* m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap;
	UINT m_nRtvDescriptorIncrementSize;
	// ����Ÿ��, �������� ������. ���� Ÿ�� ������ ũ��

	ID3D12Resource* m_pd3dDepthStencilBuffer;
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap;
	UINT m_nDsvDescriptorIncrementSize;
	// ���� ���ٽ� ����, ������ �� ������. ���� ���ٽ� ������ ũ��

	ID3D12CommandQueue* m_pd3dCommandQueue;
	ID3D12CommandAllocator* m_pd3dCommandAllocator;
	ID3D12GraphicsCommandList* m_pd3dCommandList;
	//��� ť, ��� �Ҵ���, ��� ����Ʈ ������

	ID3D12PipelineState* m_pd3dPipelineState;
	// �׷��� ���������� ���� ��ü ������

	ID3D12Fence* m_pd3dFence;
	UINT64 m_nFenceValue;
	HANDLE m_hFenceEvent;
	// �潺 ������, �潺 ��, �̺�Ʈ �ڵ�

	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;
	//����Ʈ�� ���� �簢��

};