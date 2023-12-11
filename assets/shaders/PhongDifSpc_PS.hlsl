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
    // normalize the mesh normal
    input.viewNormal = normalize(input.viewNormal);
    // fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, input.viewFragPos);
    // specular parameters
    float specularPowerLoaded = specularGloss;
    const float4 specularSample = specularTexture.Sample(anisotropicWrapSampler, input.tc);
    float3 specularReflectionColor;
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
        specularPowerLoaded = pow(2.0f, specularSample.a * 13.0f);
    }
    // attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
    // diffuse light
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, input.viewNormal);
    // specular reflected
    const float3 specularReflected = Speculate(
        diffuseColor * specularReflectionColor, specularWeight, input.viewNormal,
        lv.vToL, input.viewFragPos, att, specularPowerLoaded
    );
	// final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((diffuse + ambientColor) * diffuseTexture.Sample(anisotropicWrapSampler, input.tc).rgb + specularReflected), 1.0f);
}
