#pragma once

#include <type_traits>

#include "Math/Vector2.h"
#include "Math/Vector4.h"

struct VertexPosition
{
    Vector3 position;
};

struct VertexPositionTexture
{
    Vector3 position;
    Vector2 textureCoordinates;
};

struct VertexPositionTextureColor
{
    Vector3 position;
    Vector2 textureCoordinates;
    Vector4 color;
};

struct VertexPositionColor
{
    Vector3 position;
    Vector4 color;
};

struct VertexPositionNormal
{
    Vector3 position;
    Vector3 normal;
};

struct VertexPositionTextureNormalTangent
{
    Vector3 position;
    Vector2 textureCoordinates;
    Vector3 normal;
    Vector3 tangent;
};

static_assert(std::is_trivially_copyable<VertexPosition>::value, "RHI_Vertex_Pos is not trivially copyable");
static_assert(std::is_trivially_copyable<VertexPositionTexture>::value, "RHI_Vertex_PosTex is not trivially copyable");
static_assert(std::is_trivially_copyable<VertexPositionColor>::value, "RHI_Vertex_PosCol is not trivially copyable");
static_assert(std::is_trivially_copyable<VertexPositionTextureNormalTangent>::value, "RHI_Vertex_PosTexNorTan is not trivially copyable");
