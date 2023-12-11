cbuffer UberConstantBuffer : register(b0)
{
	matrix model;
	matrix modelView;
	matrix modelViewProjection;
	matrix projection;
	matrix sphereMatrix;

    float blurRadius;
    float blurSigma;
    float2 blurDirection;

    float2 renderTargetResolution;
    float2 padding;
	
	float4 outlineColor;
	
	float3 specularColor;
	float padding2;
	
	float3 materialColor;
	float padding3;
	
    bool useGlossAlpha;
    bool useSpecularMap;
    bool useNormalMap;
    float padding4;
	
    float specularWeight;
    float specularGloss;
    float normalMapWeight;
	float padding5;
	float2 padding6;
};

cbuffer PointLightConstantBuffer : register(b1)
{
	matrix transform;

	float3 viewLightPos;
	float padding7;

	float3 ambientColor;
	float padding8;

	float3 diffuseColor;
	float padding9;

	float diffuseIntensity;
	float attConst;
	float attLin;
	float attQuad;
};
