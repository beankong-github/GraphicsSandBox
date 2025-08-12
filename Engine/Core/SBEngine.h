#pragma once
#include "engine.h"

class Engine_API SBEngine
{
private:
	// 외부에서 SBEngine을 생성할 수 없도록 막는다.
	SBEngine();

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

	const HWND& GetMainHwnd() { return hWnd; }



private:
	// 싱글톤
	static SBEngine* instance;

	HWND hWnd;
	Vector2 resolution;
};

SBEngine* SBEngine::instance = nullptr;