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
    // sample diffuse texture
    const float4 dtex = diffuseTexture.Sample(anisotropicWrapSampler, input.tc);
    
#ifdef MASK_BOI
    // bail if highly translucent
    clip(dtex.a < 0.1f ? -1 : 1);
    // flip normal when backface
    if (dot(input.viewNormal, input.viewFragPos) >= 0.0f)
    {
        input.viewNormal = -input.viewNormal;
    }
#endif
    
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
    // specular parameter determination (mapped or uniform)
    float3 specularReflectionColor;
    float specularPower = specularGloss;
    const float4 specularSample = specularTexture.Sample(anisotropicWrapSampler, input.tc);
    if (useSpecularMap)
    {
        specularReflectionColor = specularSample.rgb;
    }
    else
    {
        specularReflectionColor = specularColor;
    }
    if (useGlossAlpha)
    {
        specularPower = pow(2.0f, specularSample.a * 13.0f);
    }
    // attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
    // diffuse light
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, input.viewNormal);
    // specular reflected
    const float3 specularReflected = Speculate(
        diffuseColor * diffuseIntensity * specularReflectionColor, specularWeight, input.viewNormal,
        lv.vToL, input.viewFragPos, att, specularPower
    );
	// final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((diffuse + ambientColor) * dtex.rgb + specularReflected), 1.0f);
}