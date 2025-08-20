
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj;
};


struct VSInput
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PSInput VSMain(VSInput vin)
{
	PSInput result;

    result.position = mul(float4(vin.position, 1.0f), gWorldViewProj);
    result.color = vin.color;

	return result;
}

float4 PSMain(PSInput input) : SV_Target
{
	return input.color;
}
