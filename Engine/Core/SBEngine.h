#pragma once
#include "engine.h"

class Engine_API SBEngine
{
private:
	// �ܺο��� SBEngine�� ������ �� ������ ���´�.
	SBEngine();

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

	const HWND& GetMainHwnd() { return hWnd; }



private:
	// �̱���
	static SBEngine* instance;

	HWND hWnd;
	Vector2 resolution;
};

SBEngine* SBEngine::instance = nullptr;