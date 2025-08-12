#include "pch.h"
#include "Device.h"


bool Device::Init()
{
	HRESULT Result = D3D12CreateDevice(
		,nullptr				// 디스플레이 어뎁터. nullptr->기본 어뎁터
		,D3D_FEATURE_LEVEL_11_0 // 최소 기능 수준(DX11)
		,this
		,
	)
}