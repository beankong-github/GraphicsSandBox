#pragma once

// ���� ��������� ���ԵǾ�� �ϴ� ������� �����Ѵ�.

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


// ����ó��
inline std::wstring AnsiToWString(const std::string& str)
{
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
    return std::wstring(buffer);
}

// Util �Լ��� ====================================

// 01. ����� -------------------------------------
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