#include "pch.h"
#include "SBEngine.h"
#include "Device.h"


SBEngine* SBEngine::instance = nullptr;

SBEngine::~SBEngine()
{
}

void SBEngine::Init(HWND hWnd, Vector2 resolution)
{
	this->hWnd = hWnd;
	this->resolution = resolution;

	// 윈도우 화면 사이즈 조절
	RECT rt = { 0, 0, resolution.x, resolution.y };
	AdjustWindowRect(&rt, WS_OVERLAPPED, false);
	// 대충 화면 중간에 오도록 조절한다.
	SetWindowPos(this->hWnd, nullptr, 10, 10, rt.right - rt.left, rt.bottom - rt.top, 0);

	Device->Init(resolution);


	return;
}

void SBEngine::Progress()
{
}
