#include "common.hlsl"

float4 mainVS(float3 pos : Position) : SV_Position
{
	return mul(float4(pos, 1.0f), transform);
}
