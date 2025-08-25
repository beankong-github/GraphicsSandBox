#pragma once
#include "engine.h"
#include "GameTimer.h"

// TMP
#include "Device.h"

class Engine_API SBEngine
{
private:
	// �ܺο��� SBEngine�� ������ �� ������ ���´�.
	SBEngine() = default;

public:
	~SBEngine();

	// singleton ��ü ��ȯ
	static SBEngine* Get()
	{
		if (instance == nullptr)
			instance = new SBEngine();

		return instance;
	}

public:
	void Init(HWND hWnd, Vector2 resolution);
	void Progress();
	void ShutDown();

	void OnResizeWindow(Vector2 resolution);

	// Window �̺�Ʈ ó��
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	inline const HWND& GetMainHwnd() { return hWnd; }
	inline const Vector2& GetResolution() { return resolution; }
	//inline const class Device* GetDevice() const { return mDevice; }
	// TMP : : �ܺο��� Device�� ������ �ʿ���� ���� ����
	inline const ComPtr<class ID3D12Device> GetDevice() const { assert(mDevice);  return mDevice->mDevice; }
	// TMP :: �ܺο��� Command List�� ������ �� ���� ����
	inline const ComPtr<class ID3D12GraphicsCommandList> GetCommandList() const { assert(mDevice);  return mDevice->mCommandList; }
	
	//TMP
	void StartWriteCommadList() { mDevice->StartWriteCommandList(); }
	void FinishAndFlushCommandList() { mDevice->FinishWriteCommandListAndFlush(); }

private:
	// �̱���
	static SBEngine* instance;

	class Device*	 mDevice;
	GameTimer		mTimer;

	HWND hWnd = 0;
	Vector2 resolution = Vector2::Zero;
};