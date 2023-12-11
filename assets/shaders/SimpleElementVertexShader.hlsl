/*=============================================================================
	SimpleElementVertexShader.hlsl: Vertex shader for drawing simple elements.
=============================================================================*/

#include "Common.hlsl"

void mainVS(
	in float3 inRelativePosition : POSITION,
	in float2 inTextureCoordinate : TEXCOORD0,
	in float4 inColor : COLOR,
	out float2 outTextureCoordinate : TEXCOORD0,
	out float4 outColor : COLOR,
	out float4 outPosition : SV_POSITION
	)
{
	outPosition = mul(float4(inRelativePosition, 1.0f), modelViewProjection);
	outTextureCoordinate = inTextureCoordinate;
	outColor = inColor;
}
