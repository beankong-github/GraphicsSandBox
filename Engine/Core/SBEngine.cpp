#include "pch.h"
#include "SBEngine.h"

SBEngine::SBEngine(Vector2 inResolution)
	:resolution(inResolution)
{

}

SBEngine::~SBEngine()
{
}

int SBEngine::Init(HWND hWnd, Vector2 resolution)
{
	this->hWnd = hWnd;
	this->resolution = resolution;

	// ������ ����


	return 0;
}
