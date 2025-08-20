#pragma once

#include <DirectXMath.h>
using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Pos;				// 12byte	
	//XMFLOAT3 Normal;		// 12byte;
	XMFLOAT4 Color;			// 16byte
	//XMFLOAT2 Texture;		 // 8byte
};