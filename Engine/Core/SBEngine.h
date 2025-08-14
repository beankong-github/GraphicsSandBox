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

	inline const HWND& GetMainHwnd() { return hWnd; }
	inline const Vector2& GetResolution() { return resolution; }


private:
	// �̱���
	static SBEngine* instance;

	ComPtr<class Device> Device;

	HWND hWnd = 0;
	Vector2 resolution = Vector2::Zero;
};