#pragma once

// ���� ��������� ���ԵǾ�� �ϴ� ������� �����Ѵ�.
#include "SimpleMath.h"
using namespace DirectX;
using namespace SimpleMath;

#ifdef ExportEngineDLL
#define Engine_API __declspec(dllexport)
#else
#define Engine_API __declspec(dllimport)
#endif
