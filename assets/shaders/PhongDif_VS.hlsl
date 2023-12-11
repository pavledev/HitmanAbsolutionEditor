#include "Common.hlsl"

struct VS_INPUT
{
    float3 pos : POSITION;
    float3 n : NORMAL;
    float2 tc : TEXCOORD;
};

struct VS_OUTPUT
{
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
    float2 tc : TEXCOORD;
    float4 pos : SV_POSITION;
};

VS_OUTPUT mainVS(const VS_INPUT input)
{
    VS_OUTPUT output;
    output.viewPos = (float3)mul(float4(input.pos, 1.0f), modelView);
    output.viewNormal = mul(input.n, (float3x3)modelView);
    output.pos = mul(float4(input.pos, 1.0f), modelViewProjection);
    output.tc = input.tc;
    return output;
}
