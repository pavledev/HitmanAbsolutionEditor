#include "Common.hlsl"

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    //float2 textureCoordinates : TEXCOORD;
	//float4 vertexColor : COLOR;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float2 textureCoordinates : TEXCOORD;
};

struct PSInput
{
	float4 position : SV_POSITION;
    float4 eye : POSITION0;
	float4 worldPosition : POSITION1;
    float3 normal : NORMAL;
	float2 textureCoordinates : TEXCOORD0;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	//float4 vertexColor : COLOR; //Solid color (for debug)
};

PSInput mainVS(VSInput input)
{
    PSInput output;
	
	//Set position into world space	
    output.position = mul(float4(input.position, 1.f), world);
	
    float3 eye = eyePosition - output.position.xyz;
    output.eye = float4(normalize(eye), length(eye)); //Use worldPosition for camera->vertex direction
	
	//Set normal for interpolation	
    output.normal = normalize(mul(input.normal, (float3x3) world));
	
    output.textureCoordinates = input.textureCoordinates;
    //output.vertexColor = input.vertexColor;
    output.worldPosition = output.position;
	
	//Set position into clip space	
    output.position = mul(output.position, viewProjection);
	
    if (hasNormalMap && !enableAutoTangent)
    {
        //Transform the tangents by the world matrix and normalize
        output.tangent = normalize(mul(input.tangent, (float3x3) world));
        output.binormal = normalize(mul(input.binormal, (float3x3) world));
    }
	
    return output;
}
