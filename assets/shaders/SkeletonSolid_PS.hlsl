#include "Common.hlsl"
#include "Utility.hlsl"
#include "LightVectorData.hlsl"

struct PS_INPUT
{
    float3 viewFragPos : POSITION;
    float3 viewNormal : NORMAL;
};

float4 mainPS(PS_INPUT input) : SV_Target
{
  /* Do lighting at an angle to avoid flat shading on front facing bone. */
  const vec3 light = vec3(0.1, 0.1, 0.8);
  float n = dot(viewNormal, light);

  /* Smooth lighting factor. */
  const float s = 0.2; /* [0.0-0.5] range */
  float fac = clamp((n * (1.0 - s)) + s, 0.0, 1.0);
  finalColor.rgb = lerp(state_color.rgb, bone_color.rgb, fac * fac);
  finalColor.a = 1.0;
  
  return finalColor;
}
