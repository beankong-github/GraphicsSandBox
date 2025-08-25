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

	// ������ ȭ�� ������ ����
	RECT rt = { 0, 0, resolution.x, resolution.y };
	AdjustWindowRect(&rt, WS_OVERLAPPED, false);
	// ���� ȭ�� �߰��� ������ �����Ѵ�.
	SetWindowPos(this->hWnd, nullptr, 10, 10, rt.right - rt.left, rt.bottom - rt.top, 0);

	mDevice = new Device();
	assert(mDevice);
	mDevice->Init();


	return;
}

void SBEngine::Progress()
{
	mDevice->Draw();
}

void SBEngine::ShutDown()
{
	SafeDelete(mDevice);

	if (instance)
	{
		delete instance;
		instance = nullptr;
	}
}

void SBEngine::OnResizeWindow(Vector2 resolution)
{
	assert(mDevice, "Device�� �������� �ʽ��ϴ�.");
	mDevice->OnResizeWindow();


}

LRESULT SBEngine::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case  WM_SIZE:
		resolution = Vector2(LOWORD(lParam), HIWORD(lParam));

		if (mDevice)
		{
			if (wParam == SIZE_MINIMIZED)
			{	
				
			}
		}
	
	}
	return LRESULT();
}
