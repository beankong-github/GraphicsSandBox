#pragma once

#include <Windows.h>
#include <d3d12.h>
#include "engine.h"
#include <D3Dcompiler.h>
#include <DirectXHelpers.h>
#include <dxgi1_4.h>
#include <comdef.h>

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