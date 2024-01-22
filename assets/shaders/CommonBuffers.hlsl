#pragma pack_matrix( row_major )

#include "DataStructs.hlsl"

cbuffer CameraConstantBuffer : register(b0)
{
    float4x4 viewProjection;
    float3 eyePosition; //Camera position
    float padding;
};

cbuffer MeshConstantBuffer : register(b1)
{
    float4x4 world;
    float4x4 modelViewProjection;
	
	bool hasDiffuseMap;
	bool hasNormalMap;
	bool hasSpecularMap;
	bool hasEmissiveMap;
	bool hasAlphaMap;
    float vertexColorBlending = 0.f;
    bool renderFlat = false; //Enable flat normal rendering
    bool enableAutoTangent = false;
    
    float4 materialDiffuse = 0.5f; //Kd := surface material's diffuse coefficient
    float4 materialAmbient = 0.25f; //Ka := surface material's ambient coefficient.
    float4 materialEmissive = 0.f; //Ke := surface material's emissive coefficient
    float4 materialSpecular = 0.f; //Ks := surface material's specular coefficient.
    float4 materialReflect = 0.f; //Kr := surface material's reflectivity coefficient.
    float materialShininess = 1.f; //Ps := surface material's shininess
    float3 padding2;
    
    float blurRadius;
    float blurSigma;
    float2 blurDirection;

    float2 renderTargetResolution;
    float2 padding3;
	
	float4 outlineColor;
	float4 meshColor;
};

cbuffer LightConstantBuffer : register(b2)
{
    Light lights[LIGHTS];
    float4 lightAmbient = float4(0.2f, 0.2f, 0.2f, 1.0f);
    int lightCount;
    float3 padding5;
};
