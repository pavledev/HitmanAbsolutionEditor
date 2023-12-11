/*=============================================================================
	SimpleElementPixelShader.hlsl: Pixel shader for drawing simple elements.
=============================================================================*/

#include "Common.hlsl"
#include "ColorUtility.hlsl"

float4 textureComponentReplicate = float4(0.f, 0.f, 0.f, 0.f);
float4 textureComponentReplicateAlpha = float4(0.f, 0.f, 0.f, 1.f);

void ReplicateChannelSimpleElementShader(inout float4 baseColor)
{
	ReplicateChannel(baseColor, textureComponentReplicate, textureComponentReplicateAlpha);
}

float4 ColourTexture2DSample(Texture2D tex, SamplerState samplerState, float2 uv)
{
	float4 sample = tex.Sample(samplerState, uv);
	
	return sample;
}

void mainPS(
	in float2 textureCoordinate : TEXCOORD0,
	in float4 color : COLOR,
	out float4 outColor : SV_Target0
	)
{
	outColor = color;
}
