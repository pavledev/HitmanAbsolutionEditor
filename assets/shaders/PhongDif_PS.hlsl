#include "Common.hlsl"
#include "Utility.hlsl"
#include "LightVectorData.hlsl"

struct PS_INPUT
{
    float3 viewFragPos : POSITION;
    float3 viewNormal : NORMAL;
    float2 tc : TEXCOORD;
};

float4 mainPS(PS_INPUT input) : SV_Target
{
	const float4 dtex = diffuseTexture.Sample(anisotropicWrapSampler, input.tc);
	clip(dtex.a < 0.1f ? -1 : 1);
    // flip normal when backface
    if (dot(input.viewNormal, input.viewFragPos) >= 0.0f)
    {
        input.viewNormal = -input.viewNormal;
    }
	
    // renormalize interpolated normal
    input.viewNormal = normalize(input.viewNormal);
    // fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, input.viewFragPos);
    // attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
    // diffuse
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, input.viewNormal);
    // specular
    const float3 specular = Speculate(diffuseColor * diffuseIntensity * specularColor, specularWeight, input.viewNormal, lv.vToL, input.viewFragPos, att, specularGloss);
	// final color
    return float4(saturate((diffuse + ambientColor) * diffuseTexture.Sample(anisotropicWrapSampler, input.tc).rgb + specular), 1.0f);
}
