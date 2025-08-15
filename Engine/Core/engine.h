#pragma once

// 엔진 헤더에서도 포함되어야 하는 내용들을 저장한다.

// math
#include "SimpleMath.h"
using namespace DirectX::SimpleMath;

// comptr
#include <wrl/client.h>
using namespace Microsoft::WRL;

// stl
#include <string>
#include <memory>

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


// 02. Com Ptr -------------------------------------
#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif