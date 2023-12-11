#pragma once

#include "Math/Matrix44.h"

struct UberConstantBuffer
{
    Matrix44 model;
    Matrix44 modelView;
    Matrix44 modelViewProjection;
    Matrix44 projection;
    Matrix44 sphereMatrix;

    float blurRadius = 5.0f;
    float blurSigma = 0.0f;
    Vector2 blurDirection;

    Vector2 renderTargetResolution;
    Vector2 padding;

    Vector4 outlineColor;

    Vector3 specularColor;
    float padding2;

    Vector3 materialColor;
    float padding3;

    bool useGlossAlpha;
    bool useSpecularMap;
    bool useNormalMap;
    float padding4;

    float specularWeight;
    float specularGloss;
    float normalMapWeight;
    float padding5;
    Vector2 padding6;
};

struct PointLightConstantBuffer
{
    Matrix44 transform;

    Vector3 viewLightPos;
	float padding;

    Vector3 ambientColor;
	float padding2;

    Vector3 diffuseColor;
	float padding3;

	float diffuseIntensity;
	float attConst;
	float attLin;
	float attQuad;
};
