#pragma once

// 엔진 헤더에서도 포함되어야 하는 내용들을 저장한다.
#include "SimpleMath.h"
using namespace DirectX;
using namespace SimpleMath;

#ifdef ExportEngineDLL
#define Engine_API __declspec(dllexport)
#else
#define Engine_API __declspec(dllimport)
#endif
