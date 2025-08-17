#pragma once
#include "engine.h"

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
	void OnResizeWindow(Vector2 resolution);

	// Window 이벤트 처리
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	inline const HWND& GetMainHwnd() { return hWnd; }
	inline const Vector2& GetResolution() { return resolution; }
	inline const class Device* GetDevice() const { return mDevice; }

private:
	// 싱글톤
	static SBEngine* instance;

	class Device* mDevice;

	HWND hWnd = 0;
	Vector2 resolution = Vector2::Zero;
};