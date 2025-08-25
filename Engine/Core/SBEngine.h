#pragma once
#include "engine.h"
#include "GameTimer.h"

// TMP
#include "Device.h"

class Engine_API SBEngine
{
private:
	// 외부에서 SBEngine을 생성할 수 없도록 막는다.
	SBEngine() = default;

public:
	~SBEngine();

	// singleton 객체 반환
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

	// Window 이벤트 처리
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	inline const HWND& GetMainHwnd() { return hWnd; }
	inline const Vector2& GetResolution() { return resolution; }
	//inline const class Device* GetDevice() const { return mDevice; }
	// TMP : : 외부에서 Device를 가져올 필요없게 구조 수정
	inline const ComPtr<class ID3D12Device> GetDevice() const { assert(mDevice);  return mDevice->mDevice; }
	// TMP :: 외부에서 Command List를 가져올 수 없게 수정
	inline const ComPtr<class ID3D12GraphicsCommandList> GetCommandList() const { assert(mDevice);  return mDevice->mCommandList; }
	
	//TMP
	void StartWriteCommadList() { mDevice->StartWriteCommandList(); }
	void FinishAndFlushCommandList() { mDevice->FinishWriteCommandListAndFlush(); }

private:
	// 싱글톤
	static SBEngine* instance;

	class Device*	 mDevice;
	GameTimer		mTimer;

	HWND hWnd = 0;
	Vector2 resolution = Vector2::Zero;
};