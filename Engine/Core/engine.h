#pragma once

// ���� ��������� ���ԵǾ�� �ϴ� ������� �����Ѵ�.

// math
#include "SimpleMath.h"
using namespace DirectX::SimpleMath;

// comptr
#include <wrl/client.h>
using namespace Microsoft::WRL;

#ifdef ExportEngineDLL
#define Engine_API __declspec(dllexport)
#else
#define Engine_API __declspec(dllimport)
#endif
