#pragma once

#include <vector>

#include "../vertex.h"

class Geometry
{
public:
    static void CreateCube(std::vector<VertexPositionTextureNormalTangent>& vertices, std::vector<unsigned short>& indices);
    static void CreateCube(std::vector<VertexPositionColor>& vertices, std::vector<unsigned short>& indices, const Vector4& color);
    static void CreateCube(std::vector<VertexPositionColor>& vertices, std::vector<unsigned short>& indices, const Vector4& color, const Vector3& position, const Vector3& dimensions);
    static void CreateQuad(std::vector<VertexPositionTextureNormalTangent>& vertices, std::vector<unsigned short>& indices);
    static void CreateCylinder(std::vector<VertexPositionTextureNormalTangent>& vertices, std::vector<unsigned short>& indices, const float radiusTop = 1.0f, const float radiusBottom = 1.0f, const float height = 1.0f, const unsigned short slices = 15, const unsigned short stacks = 15);
    static void CreateCone(std::vector<VertexPositionTextureNormalTangent>& vertices, std::vector<unsigned short>& indices, const float radius = 1.0f, const float height = 2.0f);

    template <typename T>
    static void CreateSphere(std::vector<T>& vertices, std::vector<unsigned short>& indices, const float radius = 1.0f, const unsigned short slices = 20, const unsigned short stacks = 20);

    template <>
    static void CreateSphere(std::vector<VertexPosition>& vertices, std::vector<unsigned short>& indices, const float radius, const unsigned short slices, const unsigned short stacks);

    template <>
    static void CreateSphere(std::vector<VertexPositionNormal>& vertices, std::vector<unsigned short>& indices, const float radius, const unsigned short slices, const unsigned short stacks);

    template <>
    static void CreateSphere(std::vector<VertexPositionTextureNormalTangent>& vertices, std::vector<unsigned short>& indices, const float radius, const unsigned short slices, const unsigned short stacks);

    static void CreateOctahedralBone(std::vector<VertexPositionNormal>& vertices);
    static void CreateOctahedralBone(std::vector<VertexPosition>& vertices, std::vector<unsigned short>& indices);
    static void CreateCircle(std::vector<VertexPositionColor>& vertices, const Vector3& base, const Vector3& xAxis, const Vector3& yAxis, const Vector4& color, double radius, int numSides);
    static void CreateHalfCircle(std::vector<VertexPositionColor>& vertices, const Vector3& base, const Vector3& xAxis, const Vector3& yAxis, const Vector4& color, double radius, int numSides);
    static void CreateWireCapsule(std::vector<VertexPositionColor>& vertices, const Vector3& base, const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis, const Vector4& color, double radius, double HalfHeight, int NumSides);
};
