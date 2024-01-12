#pragma pack_matrix( row_major )

#define LIGHTS 8

struct Light
{
    int lightType;
    float3 padding;
	//The light direction is here the vector which looks towards the light
    float4 lightDirection;
    float4 lightPosition;
    float4 lightAttenuation;
    float4 lightSpot; //(outer angle, inner angle, falloff, free)
    float4 lightColor;
};
