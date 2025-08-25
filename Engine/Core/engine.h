#pragma once

// 엔진 헤더에서도 포함되어야 하는 내용들을 저장한다.

// d3d
#include <d3dcommon.h>
#include <d3dx12.h>

// math
#include "SimpleMath.h"
using namespace DirectX::SimpleMath;

// comptr
#include <wrl/client.h>
using namespace Microsoft::WRL;

// stl
#include <string>
#include <memory>
using namespace std;

//SBEngine
#include "struct.h"

#ifdef ExportEngineDLL
#define Engine_API __declspec(dllexport)
#else
#define Engine_API __declspec(dllimport)
#endif


// 예외처리
inline std::wstring AnsiToWString(const std::string& str)
{
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
    return std::wstring(buffer);
}

struct ObjectConstants
{
    XMFLOAT4X4 WorldViewProj = Matrix::Identity;
};

// Util 함수들 ====================================

// 01. 디버그 -------------------------------------
#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw SBException(hr__, L#x, wfn, __LINE__); } \
}
#endif

class SBException
{
public:
    SBException() = default;
    SBException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

    std::wstring ToString()const;

    HRESULT ErrorCode = S_OK;
    std::wstring FunctionName;
    std::wstring Filename;
    int LineNumber = -1;
};


// 02. Com Ptr / Memory -------------------------------------
#ifndef SafeRelease
#define SafeRelease(x) { if(x){ x->Release(); x = 0; } }
#endif

template<typename T>
inline void SafeReleaseArray(T** arr, size_t count)
{
    if (!arr) return;
    for (size_t i = 0; i < count; ++i)
    {
        SafeRelease(arr[i]);
    }
}

template<typename T>
void SafeDelete(T*& obj)
{
    if (obj != nullptr)
    {
        delete obj;
        obj = nullptr;
    }
}

static UINT CalcConstantBufferByteSize(UINT byteSize)
    {
        // Constant buffers must be a multiple of the minimum hardware
        // allocation size (usually 256 bytes).  So round up to nearest
        // multiple of 256.  We do this by adding 255 and then masking off
        // the lower 2 bytes which store all bits < 256.
        // Example: Suppose byteSize = 300.
        // (300 + 255) & ~255
        // 555 & ~255
        // 0x022B & ~0x00ff
        // 0x022B & 0xff00
        // 0x0200
        // 512
        return (byteSize + 255) & ~255;
    }



// 03. Graphics  ---------------------------------------

ID3D12Resource* CreateDefaultBuffer(
    ID3D12Device* device,
    ID3D12GraphicsCommandList* cmdList,
    const void* initData,
    UINT64 byteSize,
    ID3D12Resource** uploadBuffer);

ID3DBlob* CompileShader(
    const std::wstring& filename,
    const D3D_SHADER_MACRO* defines,
    const std::string& entrypoint,
    const std::string& target);

static void LogShaderError(HRESULT hr, ID3DBlob* errors);

