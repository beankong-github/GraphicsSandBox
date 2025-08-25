#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외
#endif
#include <Windows.h>

// d3dx
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXHelpers.h>
#include <dxgi1_4.h>
#include <comdef.h>
#include <DirectXColors.h>

// engine global
#include "Core/engine.h"

// 메모리 릭
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif


// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")


// namespace
using namespace DirectX;