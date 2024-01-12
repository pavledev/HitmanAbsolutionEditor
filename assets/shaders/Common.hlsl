#include "CommonVertexPixel.hlsl"
#include "CommonBuffers.hlsl"
#include "CommonSampler.hlsl"
#include "CommonTexture.hlsl"

static const uint THREAD_GROUP_COUNT_X = 8;
static const uint THREAD_GROUP_COUNT_Y = 8;

// Christian Schüler, "Normal Mapping without Precomputed Tangents", ShaderX 5, Chapter 2.6, pp. 131 – 140
// See also follow-up blog post: http://www.thetenthplanet.de/archives/1180
float3x3 CalculateTBN(float3 p, float3 n, float2 tex)
{
    float3 dp1 = ddx(p);
    float3 dp2 = ddy(p);
    float2 duv1 = ddx(tex);
    float2 duv2 = ddy(tex);

    float3x3 M = float3x3(dp1, dp2, cross(dp1, dp2));
    float2x3 inverseM = float2x3(cross(M[1], M[2]), cross(M[2], M[0]));
    float3 t = normalize(mul(float2(duv1.x, duv2.x), inverseM));
    float3 b = normalize(mul(float2(duv1.y, duv2.y), inverseM));
    return float3x3(t, b, n);
}

float3 BiasX2(float3 x)
{
    return 2.0f * x - 1.0f;
}

float3 PeturbNormal(float3 localNormal, float3 position, float3 normal, float2 texCoord)
{
    const float3x3 TBN = CalculateTBN(position, normal, texCoord);
    return normalize(mul(localNormal, TBN));
}
