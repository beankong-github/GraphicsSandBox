#pragma once
#include "engine.h"

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
	void OnResizeWindow(Vector2 resolution);

	// Window �̺�Ʈ ó��
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	inline const HWND& GetMainHwnd() { return hWnd; }
	inline const Vector2& GetResolution() { return resolution; }
	inline const class Device* GetDevice() const { return mDevice; }

private:
	// �̱���
	static SBEngine* instance;

	class Device* mDevice;

	HWND hWnd = 0;
	Vector2 resolution = Vector2::Zero;
};