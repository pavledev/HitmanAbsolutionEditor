#include "Common.hlsl"
#include "Utility.hlsl"
#include "LightVectorData.hlsl"

struct PS_INPUT
{
    float3 viewFragPos : POSITION;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float3 viewBinormal : BINORMAL;
    float2 tc : TEXCOORD;
};

float4 mainPS(PS_INPUT input) : SV_Target
{
    // normalize the mesh normal
    input.viewNormal = normalize(input.viewNormal);
    // replace normal with mapped if normal mapping enabled
    if (useNormalMap)
    {
        const float3 mappedNormal = MapNormal(normalize(input.viewTangent), normalize(input.viewBinormal), input.viewNormal, input.tc, normalTexture, anisotropicWrapSampler);
        input.viewNormal = lerp(input.viewNormal, mappedNormal, normalMapWeight);
    }
    // fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, input.viewFragPos);
    // attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
    // diffuse
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, input.viewNormal);
    // specular
    const float3 specular = Speculate(
        diffuseColor * diffuseIntensity * specularColor, specularWeight, input.viewNormal,
        lv.vToL, input.viewFragPos, att, specularGloss
    );
	// final color
    return float4(saturate((diffuse + ambientColor) * diffuseTexture.Sample(anisotropicWrapSampler, input.tc).rgb + specular), 1.0f);
}
