#pragma once
#include "engine.h"

class  Engine_API SBEngine
{
public:
	SBEngine(Vector2  inResolution);
	~SBEngine();

public:
	int Init(HWND hWnd, Vector2 resolution);
	void Progress();

	const HWND& GetMainHwnd() { return hWnd; }


private:
	HWND hWnd;
	Vector2 resolution;
};
