#pragma once

#include <Windows.h>

#include "Math/Matrix.h"
#include "Math/Vector2.h"

#define LIGHTS 8

struct CameraConstantBuffer
{
    Matrix viewProjection;
    Vector3 eyePosition; //Camera position
    float padding;
};

struct MeshConstantBuffer
{
    Matrix world;
    Matrix modelViewProjection;

    BOOL hasDiffuseMap;
    BOOL hasNormalMap;
    BOOL hasSpecularMap;
    BOOL hasEmissiveMap;
    BOOL hasAlphaMap;
    float vertexColorBlending = 0.f;
    BOOL renderFlat = false; //Enable flat normal rendering
    BOOL enableAutoTangent = false;

    Vector4 materialDiffuse = 0.5f; //Kd := surface material's diffuse coefficient
    Vector4 materialAmbient = 0.25f; //Ka := surface material's ambient coefficient.
    Vector4 materialEmissive = 0.f; //Ke := surface material's emissive coefficient
    Vector4 materialSpecular = 0.f; //Ks := surface material's specular coefficient.
    Vector4 materialReflect = 0.f; //Kr := surface material's reflectivity coefficient.
    float materialShininess = 1.f; //Ps := surface material's shininess
    Vector3 padding;

    float blurRadius;
    float blurSigma;
    Vector2 blurDirection;

    Vector2 renderTargetResolution;
    Vector2 padding2;

    Vector4 outlineColor;

    Vector3 materialColor;
    float padding3;
};

struct Light
{
    int lightType;
    Vector3 padding;
    //The light direction is here the vector which looks towards the light
    Vector4 lightDirection;
    Vector4 lightPosition;
    Vector4 lightAttenuation;
    Vector4 lightSpot; //(outer angle, inner angle, falloff, free)
    Vector4 lightColor;
};

struct LightConstantBuffer
{
    Light lights[LIGHTS];
    Vector4 lightAmbient = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
    int lightCount;
    Vector3 padding;
};
