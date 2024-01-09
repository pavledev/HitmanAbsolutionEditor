#pragma once

#include "Math/Matrix.h"
#include "Math/Vector2.h"

struct UberConstantBuffer
{
    Matrix model;
    Matrix modelView;
    Matrix modelViewProjection;
    Matrix projection;
    Matrix sphereMatrix;

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
    Matrix transform;

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
