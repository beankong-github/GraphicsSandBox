#include "pch.h"
#include "Device.h"


bool Device::Init()
{
	HRESULT Result = D3D12CreateDevice(
		,nullptr				// ���÷��� ���. nullptr->�⺻ ���
		,D3D_FEATURE_LEVEL_11_0 // �ּ� ��� ����(DX11)
		,this
		,
	)
}