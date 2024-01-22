#include <DirectXMath.h>

#include "Rendering/Scene/Geometry.h"
#include "Utility/Math.h"

void Geometry::CreateCube(std::vector<VertexPositionTextureNormalTangent>& vertices, std::vector<unsigned short>& indices)
{
    // front
    vertices.emplace_back(Vector3(-0.5f, -0.5f, -0.5f), Vector2(0, 1), Vector3(0, 0, -1), Vector3(0, 1, 0));
    vertices.emplace_back(Vector3(-0.5f, 0.5f, -0.5f), Vector2(0, 0), Vector3(0, 0, -1), Vector3(0, 1, 0));
    vertices.emplace_back(Vector3(0.5f, -0.5f, -0.5f), Vector2(1, 1), Vector3(0, 0, -1), Vector3(0, 1, 0));
    vertices.emplace_back(Vector3(0.5f, 0.5f, -0.5f), Vector2(1, 0), Vector3(0, 0, -1), Vector3(0, 1, 0));

    // bottom
    vertices.emplace_back(Vector3(-0.5f, -0.5f, 0.5f), Vector2(0, 1), Vector3(0, -1, 0), Vector3(1, 0, 0));
    vertices.emplace_back(Vector3(-0.5f, -0.5f, -0.5f), Vector2(0, 0), Vector3(0, -1, 0), Vector3(1, 0, 0));
    vertices.emplace_back(Vector3(0.5f, -0.5f, 0.5f), Vector2(1, 1), Vector3(0, -1, 0), Vector3(1, 0, 0));
    vertices.emplace_back(Vector3(0.5f, -0.5f, -0.5f), Vector2(1, 0), Vector3(0, -1, 0), Vector3(1, 0, 0));

    // back
    vertices.emplace_back(Vector3(-0.5f, -0.5f, 0.5f), Vector2(1, 1), Vector3(0, 0, 1), Vector3(0, 1, 0));
    vertices.emplace_back(Vector3(-0.5f, 0.5f, 0.5f), Vector2(1, 0), Vector3(0, 0, 1), Vector3(0, 1, 0));
    vertices.emplace_back(Vector3(0.5f, -0.5f, 0.5f), Vector2(0, 1), Vector3(0, 0, 1), Vector3(0, 1, 0));
    vertices.emplace_back(Vector3(0.5f, 0.5f, 0.5f), Vector2(0, 0), Vector3(0, 0, 1), Vector3(0, 1, 0));

    // top
    vertices.emplace_back(Vector3(-0.5f, 0.5f, 0.5f), Vector2(0, 0), Vector3(0, 1, 0), Vector3(1, 0, 0));
    vertices.emplace_back(Vector3(-0.5f, 0.5f, -0.5f), Vector2(0, 1), Vector3(0, 1, 0), Vector3(1, 0, 0));
    vertices.emplace_back(Vector3(0.5f, 0.5f, 0.5f), Vector2(1, 0), Vector3(0, 1, 0), Vector3(1, 0, 0));
    vertices.emplace_back(Vector3(0.5f, 0.5f, -0.5f), Vector2(1, 1), Vector3(0, 1, 0), Vector3(1, 0, 0));

    // left
    vertices.emplace_back(Vector3(-0.5f, -0.5f, 0.5f), Vector2(0, 1), Vector3(-1, 0, 0), Vector3(0, 1, 0));
    vertices.emplace_back(Vector3(-0.5f, 0.5f, 0.5f), Vector2(0, 0), Vector3(-1, 0, 0), Vector3(0, 1, 0));
    vertices.emplace_back(Vector3(-0.5f, -0.5f, -0.5f), Vector2(1, 1), Vector3(-1, 0, 0), Vector3(0, 1, 0));
    vertices.emplace_back(Vector3(-0.5f, 0.5f, -0.5f), Vector2(1, 0), Vector3(-1, 0, 0), Vector3(0, 1, 0));

    // right
    vertices.emplace_back(Vector3(0.5f, -0.5f, 0.5f), Vector2(1, 1), Vector3(1, 0, 0), Vector3(0, 1, 0));
    vertices.emplace_back(Vector3(0.5f, 0.5f, 0.5f), Vector2(1, 0), Vector3(1, 0, 0), Vector3(0, 1, 0));
    vertices.emplace_back(Vector3(0.5f, -0.5f, -0.5f), Vector2(0, 1), Vector3(1, 0, 0), Vector3(0, 1, 0));
    vertices.emplace_back(Vector3(0.5f, 0.5f, -0.5f), Vector2(0, 0), Vector3(1, 0, 0), Vector3(0, 1, 0));

    // front
    indices.emplace_back(0); indices.emplace_back(1); indices.emplace_back(2);
    indices.emplace_back(2); indices.emplace_back(1); indices.emplace_back(3);

    // bottom
    indices.emplace_back(4); indices.emplace_back(5); indices.emplace_back(6);
    indices.emplace_back(6); indices.emplace_back(5); indices.emplace_back(7);

    // back
    indices.emplace_back(10); indices.emplace_back(9); indices.emplace_back(8);
    indices.emplace_back(11); indices.emplace_back(9); indices.emplace_back(10);

    // top
    indices.emplace_back(14); indices.emplace_back(13); indices.emplace_back(12);
    indices.emplace_back(15); indices.emplace_back(13); indices.emplace_back(14);

    // left
    indices.emplace_back(16); indices.emplace_back(17); indices.emplace_back(18);
    indices.emplace_back(18); indices.emplace_back(17); indices.emplace_back(19);

    // right
    indices.emplace_back(22); indices.emplace_back(21); indices.emplace_back(20);
    indices.emplace_back(23); indices.emplace_back(21); indices.emplace_back(22);
}

void Geometry::CreateCube(std::vector<VertexPosition>& vertices, std::vector<unsigned short>& indices)
{
    vertices.resize(8);
    indices.resize(24);

    vertices[0].position = Vector3(-1.f, -1.f, -1.f);
    vertices[1].position = Vector3(1.f, -1.f, -1.f);
    vertices[2].position = Vector3(1.f, -1.f, 1.f);
    vertices[3].position = Vector3(-1.f, -1.f, 1.f);
    vertices[4].position = Vector3(-1.f, 1.f, -1.f);
    vertices[5].position = Vector3(1.f, 1.f, -1.f);
    vertices[6].position = Vector3(1.f, 1.f, 1.f);
    vertices[7].position = Vector3(-1.f, 1.f, 1.f);

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(3);
    indices.push_back(0);
    indices.push_back(4);
    indices.push_back(5);
    indices.push_back(5);
    indices.push_back(6);
    indices.push_back(6);
    indices.push_back(7);
    indices.push_back(7);
    indices.push_back(4);
    indices.push_back(0);
    indices.push_back(4);
    indices.push_back(1);
    indices.push_back(5);
    indices.push_back(2);
    indices.push_back(6);
    indices.push_back(3);
    indices.push_back(7);
}

void Geometry::CreateCube(std::vector<VertexPositionColor>& vertices, std::vector<unsigned short>& indices, const Vector4& color, const Vector3& position, const Vector3& dimensions)
{
    vertices.resize(8);
    indices.resize(24);

    Vector3 minVertex = position - dimensions;
    Vector3 maxVertex = position + dimensions;

    vertices[0].position = Vector3(minVertex.x, minVertex.y, minVertex.z); //Bottom front left
    vertices[1].position = Vector3(maxVertex.x, minVertex.y, minVertex.z); //Bottom front right
    vertices[2].position = Vector3(maxVertex.x, maxVertex.y, minVertex.z); //Top front right
    vertices[3].position = Vector3(minVertex.x, maxVertex.y, minVertex.z); //Top front left
    vertices[4].position = Vector3(minVertex.x, minVertex.y, maxVertex.z); //Bottom back left
    vertices[5].position = Vector3(maxVertex.x, minVertex.y, maxVertex.z); //Bottom back right
    vertices[6].position = Vector3(maxVertex.x, maxVertex.y, maxVertex.z); //Top back right
    vertices[7].position = Vector3(minVertex.x, maxVertex.y, maxVertex.z); //Top back left

    vertices[0].color = color;
    vertices[1].color = color;
    vertices[2].color = color;
    vertices[3].color = color;
    vertices[4].color = color;
    vertices[5].color = color;
    vertices[6].color = color;
    vertices[7].color = color;

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(3);
    indices.push_back(0);
    indices.push_back(4);
    indices.push_back(5);
    indices.push_back(5);
    indices.push_back(6);
    indices.push_back(6);
    indices.push_back(7);
    indices.push_back(7);
    indices.push_back(4);
    indices.push_back(0);
    indices.push_back(4);
    indices.push_back(1);
    indices.push_back(5);
    indices.push_back(2);
    indices.push_back(6);
    indices.push_back(3);
    indices.push_back(7);
}

void Geometry::CreateQuad(std::vector<VertexPositionTextureNormalTangent>& vertices, std::vector<unsigned short>& indices)
{
    vertices.emplace_back(Vector3(-0.5f, 0.0f, 0.5f), Vector2(0, 0), Vector3(0, 1, 0), Vector3(1, 0, 0)); // 0 top-left
    vertices.emplace_back(Vector3(0.5f, 0.0f, 0.5f), Vector2(1, 0), Vector3(0, 1, 0), Vector3(1, 0, 0)); // 1 top-right
    vertices.emplace_back(Vector3(-0.5f, 0.0f, -0.5f), Vector2(0, 1), Vector3(0, 1, 0), Vector3(1, 0, 0)); // 2 bottom-left
    vertices.emplace_back(Vector3(0.5f, 0.0f, -0.5f), Vector2(1, 1), Vector3(0, 1, 0), Vector3(1, 0, 0)); // 3 bottom-right

    indices.emplace_back(3);
    indices.emplace_back(2);
    indices.emplace_back(0);
    indices.emplace_back(3);
    indices.emplace_back(0);
    indices.emplace_back(1);
}

void Geometry::CreateCylinder(std::vector<VertexPositionTextureNormalTangent>& vertices, std::vector<unsigned short>& indices, const float radiusTop, const float radiusBottom, const float height, const unsigned short slices, const unsigned short stacks)
{
    const float stackHeight = height / stacks;
    const float radiusStep = (radiusTop - radiusBottom) / stacks;
    const float ringCount = (float)(stacks + 1);

    for (int i = 0; i < ringCount; i++)
    {
        const float y = -0.5f * height + i * stackHeight;
        const float r = radiusBottom + i * radiusStep;
        const float dTheta = 2.0f * DirectX::XM_PI / slices;

        for (int j = 0; j <= slices; j++)
        {
            const float c = cos(j * dTheta);
            const float s = sin(j * dTheta);

            Vector3 v = Vector3(r * c, y, r * s);
            Vector2 uv = Vector2((float)j / slices, 1.0f - (float)i / stacks);
            Vector3 t = Vector3(-s, 0.0f, c);

            const float dr = radiusBottom - radiusTop;
            Vector3 bitangent = Vector3(dr * c, -height, dr * s);

            Vector3 n = Vector3::Cross(t, bitangent).Normalized();

            vertices.emplace_back(v, uv, n, t);

        }
    }

    const int ringVertexCount = slices + 1;

    for (int i = 0; i < stacks; i++)
    {
        for (int j = 0; j < slices; j++)
        {
            indices.push_back(i * ringVertexCount + j);
            indices.push_back((i + 1) * ringVertexCount + j);
            indices.push_back((i + 1) * ringVertexCount + j + 1);

            indices.push_back(i * ringVertexCount + j);
            indices.push_back((i + 1) * ringVertexCount + j + 1);
            indices.push_back(i * ringVertexCount + j + 1);
        }
    }

    // Build top cap
    int baseIndex = (int)vertices.size();
    float y = 0.5f * height;
    const float dTheta = 2.0f * DirectX::XM_PI / slices;

    Vector3 normal;
    Vector3 tangent;

    for (int i = 0; i <= slices; i++)
    {
        const float x = radiusTop * cos(i * dTheta);
        const float z = radiusTop * sin(i * dTheta);
        const float u = x / height + 0.5f;
        const float v = z / height + 0.5f;

        normal = Vector3(0, 1, 0);
        tangent = Vector3(1, 0, 0);

        vertices.emplace_back(Vector3(x, y, z), Vector2(u, v), normal, tangent);
    }

    normal = Vector3(0, 1, 0);
    tangent = Vector3(1, 0, 0);

    vertices.emplace_back(Vector3(0, y, 0), Vector2(0.5f, 0.5f), normal, tangent);

    int centerIndex = (int)vertices.size() - 1;

    for (int i = 0; i < slices; i++)
    {
        indices.push_back(centerIndex);
        indices.push_back(baseIndex + i + 1);
        indices.push_back(baseIndex + i);
    }

    // Build bottom cap
    baseIndex = (int)vertices.size();
    y = -0.5f * height;

    for (int i = 0; i <= slices; i++)
    {
        const float x = radiusBottom * cos(i * dTheta);
        const float z = radiusBottom * sin(i * dTheta);
        const float u = x / height + 0.5f;
        const float v = z / height + 0.5f;

        normal = Vector3(0, -1, 0);
        tangent = Vector3(1, 0, 0);

        vertices.emplace_back(Vector3(x, y, z), Vector2(u, v), normal, tangent);
    }

    normal = Vector3(0, -1, 0);
    tangent = Vector3(1, 0, 0);

    vertices.emplace_back(Vector3(0, y, 0), Vector2(0.5f, 0.5f), normal, tangent);

    centerIndex = (int)vertices.size() - 1;

    for (int i = 0; i < slices; i++)
    {
        indices.push_back(centerIndex);
        indices.push_back(baseIndex + i);
        indices.push_back(baseIndex + i + 1);
    }
}

void Geometry::CreateCone(std::vector<VertexPositionTextureNormalTangent>& vertices, std::vector<unsigned short>& indices, const float radius, const float height)
{
    CreateCylinder(vertices, indices, 0.0f, radius, height);
}

template<>
void Geometry::CreateSphere(std::vector<VertexPosition>& vertices, std::vector<unsigned short>& indices, const float radius, const unsigned short slices, const unsigned short stacks)
{
    vertices.emplace_back(Vector3(0, radius, 0));

    const float phiStep = DirectX::XM_PI / stacks;
    const float thetaStep = 2.0f * DirectX::XM_PI / slices;

    for (int i = 1; i <= stacks - 1; i++)
    {
        const float phi = i * phiStep;

        for (int j = 0; j <= slices; j++)
        {
            const float theta = j * thetaStep;
            Vector3 p = Vector3(
                (radius * sin(phi) * cos(theta)),
                (radius * cos(phi)),
                (radius * sin(phi) * sin(theta))
            );

            vertices.emplace_back(p);
        }
    }

    vertices.emplace_back(Vector3(0, -radius, 0));

    for (int i = 1; i <= slices; i++)
    {
        indices.emplace_back(0);
        indices.emplace_back(i + 1);
        indices.emplace_back(i);
    }

    int baseIndex = 1;
    const int ringVertexCount = slices + 1;

    for (int i = 0; i < stacks - 2; i++)
    {
        for (int j = 0; j < slices; j++)
        {
            indices.emplace_back(baseIndex + i * ringVertexCount + j);
            indices.emplace_back(baseIndex + i * ringVertexCount + j + 1);
            indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j);

            indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j);
            indices.emplace_back(baseIndex + i * ringVertexCount + j + 1);
            indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
        }
    }

    int southPoleIndex = (int)vertices.size() - 1;
    baseIndex = southPoleIndex - ringVertexCount;

    for (int i = 0; i < slices; i++)
    {
        indices.emplace_back(southPoleIndex);
        indices.emplace_back(baseIndex + i);
        indices.emplace_back(baseIndex + i + 1);
    }
}

template<>
void Geometry::CreateSphere(std::vector<VertexPositionNormal>& vertices, std::vector<unsigned short>& indices, const float radius, const unsigned short slices, const unsigned short stacks)
{
    Vector3 normal = Vector3(0, 1, 0);

    vertices.emplace_back(Vector3(0, radius, 0), normal);

    const float phiStep = DirectX::XM_PI / stacks;
    const float thetaStep = 2.0f * DirectX::XM_PI / slices;

    for (int i = 1; i <= stacks - 1; i++)
    {
        const float phi = i * phiStep;

        for (int j = 0; j <= slices; j++)
        {
            const float theta = j * thetaStep;
            Vector3 p = Vector3(
                (radius * sin(phi) * cos(theta)),
                (radius * cos(phi)),
                (radius * sin(phi) * sin(theta))
            );

            Vector3 n = p.Normalized();

            vertices.emplace_back(p, n);
        }
    }

    normal = Vector3(0, -1, 0);

    vertices.emplace_back(Vector3(0, -radius, 0), normal);

    for (int i = 1; i <= slices; i++)
    {
        indices.emplace_back(0);
        indices.emplace_back(i + 1);
        indices.emplace_back(i);
    }

    int baseIndex = 1;
    const int ringVertexCount = slices + 1;

    for (int i = 0; i < stacks - 2; i++)
    {
        for (int j = 0; j < slices; j++)
        {
            indices.emplace_back(baseIndex + i * ringVertexCount + j);
            indices.emplace_back(baseIndex + i * ringVertexCount + j + 1);
            indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j);

            indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j);
            indices.emplace_back(baseIndex + i * ringVertexCount + j + 1);
            indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
        }
    }

    int southPoleIndex = (int)vertices.size() - 1;
    baseIndex = southPoleIndex - ringVertexCount;

    for (int i = 0; i < slices; i++)
    {
        indices.emplace_back(southPoleIndex);
        indices.emplace_back(baseIndex + i);
        indices.emplace_back(baseIndex + i + 1);
    }
}

template<>
void Geometry::CreateSphere(std::vector<VertexPositionTextureNormalTangent>& vertices, std::vector<unsigned short>& indices, const float radius, const unsigned short slices, const unsigned short stacks)
{
    Vector3 normal = Vector3(0, 1, 0);
    Vector3 tangent = Vector3(1, 0, 0);

    vertices.emplace_back(Vector3(0, radius, 0), Vector2::Zero, normal, tangent);

    const float phiStep = DirectX::XM_PI / stacks;
    const float thetaStep = 2.0f * DirectX::XM_PI / slices;

    for (int i = 1; i <= stacks - 1; i++)
    {
        const float phi = i * phiStep;

        for (int j = 0; j <= slices; j++)
        {
            const float theta = j * thetaStep;
            Vector3 p = Vector3(
                (radius * sin(phi) * cos(theta)),
                (radius * cos(phi)),
                (radius * sin(phi) * sin(theta))
            );

            Vector3 t = Vector3(-radius * sin(phi) * sin(theta), 0, radius * sin(phi) * cos(theta)).Normalized();
            Vector3 n = p.Normalized();
            Vector2 uv = Vector2(theta / (DirectX::XM_PI * 2), phi / DirectX::XM_PI);

            vertices.emplace_back(p, uv, n, t);
        }
    }

    normal = Vector3(0, -1, 0);
    tangent = Vector3(1, 0, 0);

    vertices.emplace_back(Vector3(0, -radius, 0), Vector2(0, 1), normal, tangent);

    for (int i = 1; i <= slices; i++)
    {
        indices.emplace_back(0);
        indices.emplace_back(i + 1);
        indices.emplace_back(i);
    }

    int baseIndex = 1;
    const int ringVertexCount = slices + 1;

    for (int i = 0; i < stacks - 2; i++)
    {
        for (int j = 0; j < slices; j++)
        {
            indices.emplace_back(baseIndex + i * ringVertexCount + j);
            indices.emplace_back(baseIndex + i * ringVertexCount + j + 1);
            indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j);

            indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j);
            indices.emplace_back(baseIndex + i * ringVertexCount + j + 1);
            indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
        }
    }

    int southPoleIndex = (int)vertices.size() - 1;
    baseIndex = southPoleIndex - ringVertexCount;

    for (int i = 0; i < slices; i++)
    {
        indices.emplace_back(southPoleIndex);
        indices.emplace_back(baseIndex + i);
        indices.emplace_back(baseIndex + i + 1);
    }
}

void Geometry::CreateOctahedralBone(std::vector<VertexPositionNormal>& vertices)
{
    static const Vector3 octahedralBoneVertices[6] =
    {
        { 0.0f, 0.0f, 0.0f },
        { 0.1f, 0.1f, 0.1f },
        { 0.1f, 0.1f, -0.1f },
        { -0.1f, 0.1f, -0.1f },
        { -0.1f, 0.1f, 0.1f },
        { 0.0f, 1.0f, 0.0f }
    };

    static const Vector3 octahedralBoneSmoothNormals[6] =
    {
        { 0.0f, -1.0f, 0.0f },
        { M_SQRT1_2, 0.0f, M_SQRT1_2 },
        { M_SQRT1_2, 0.0f, -M_SQRT1_2 },
        { -M_SQRT1_2, 0.0f, -M_SQRT1_2 },
        { -M_SQRT1_2, 0.0f, M_SQRT1_2 },
        { 0.0f, 1.0f, 0.0f }
    };

    static const unsigned int octahedralBoneSolidTris[8][3] =
    {
        { 2, 1, 0 }, /* bottom */
        { 3, 2, 0 },
        { 4, 3, 0 },
        { 1, 4, 0 },

        { 5, 1, 2 }, /* top */
        { 5, 2, 3 },
        { 5, 3, 4 },
        { 5, 4, 1 },
    };

    /* aligned with bone_octahedral_solid_tris */
    static const Vector3 octahedralBoneSolidNormals[8] =
    {
        { M_SQRT1_2, -M_SQRT1_2, 0.00000000f },
        { -0.00000000f, -M_SQRT1_2, -M_SQRT1_2 },
        { -M_SQRT1_2, -M_SQRT1_2, 0.00000000f },
        { 0.00000000f, -M_SQRT1_2, M_SQRT1_2 },
        { 0.99388373f, 0.11043154f, -0.00000000f },
        { 0.00000000f, 0.11043154f, -0.99388373f },
        { -0.99388373f, 0.11043154f, 0.00000000f },
        { 0.00000000f, 0.11043154f, 0.99388373f },
    };

    vertices.resize(24);

    unsigned int index = 0;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            vertices[index].position = octahedralBoneVertices[octahedralBoneSolidTris[i][j]];
            vertices[index++].normal = octahedralBoneSolidNormals[i];
        }
    }
}

void Geometry::CreateOctahedralBone(std::vector<VertexPosition>& vertices, std::vector<unsigned short>& indices)
{
    static const Vector3 octahedralBoneVertices[6] =
    {
        { 0.0f, 0.0f, 0.0f },
        { 0.1f, 0.1f, 0.1f },
        { 0.1f, 0.1f, -0.1f },
        { -0.1f, 0.1f, -0.1f },
        { -0.1f, 0.1f, 0.1f },
        { 0.0f, 1.0f, 0.0f }
    };

    static const unsigned int octahedralBoneSolidTris[8][3] =
    {
        { 2, 1, 0 }, /* bottom */
        { 3, 2, 0 },
        { 4, 3, 0 },
        { 1, 4, 0 },

        { 5, 1, 2 }, /* top */
        { 5, 2, 3 },
        { 5, 3, 4 },
        { 5, 4, 1 },
    };

    vertices.resize(6);
    indices.resize(24);

    for (int i = 0; i < 6; i++)
    {
        vertices[i].position = octahedralBoneVertices[i];
    }

    unsigned int index = 0;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            indices[index++] = octahedralBoneSolidTris[i][j];
        }
    }
}

void Geometry::CreateCircle(std::vector<VertexPositionColor>& vertices, const Vector3& base, const Vector3& x, const Vector3& y, const Vector4& color, double radius, int numSides)
{
    const float	angleDelta = 2.0f * M_PI / numSides;
    Vector3 lastVertex = base + x * radius;

    for (int sideIndex = 0; sideIndex < numSides; ++sideIndex)
    {
        const Vector3 vertex = base + (x * Math::Cos(angleDelta * (sideIndex + 1)) + y * Math::Sin(angleDelta * (sideIndex + 1))) * radius;

        vertices.push_back({ lastVertex, color });
        vertices.push_back({ vertex, color });

        lastVertex = vertex;
    }
}

void Geometry::CreateHalfCircle(std::vector<VertexPositionColor>& vertices, const Vector3& base, const Vector3& x, const Vector3& y, const Vector4& color, double radius, int numSides)
{
    const float	angleDelta = static_cast<float>(M_PI) / (static_cast<float>(numSides));
    Vector3	lastVertex = base + x * radius;

    for (int sideIndex = 0; sideIndex < numSides; ++sideIndex)
    {
        const Vector3 vertex = base + (x * Math::Cos(angleDelta * (sideIndex + 1)) + y * Math::Sin(angleDelta * (sideIndex + 1))) * radius;

        vertices.push_back({ lastVertex, color });
        vertices.push_back({ vertex, color });

        lastVertex = vertex;
    }
}

void Geometry::CreateWireCapsule(std::vector<VertexPositionColor>& vertices, const Vector3& base, const Vector3& x, const Vector3& y, const Vector3& z, const Vector4& color, double radius, double HalfHeight, int NumSides)
{
    const Vector3 origin = base;
    const Vector3 xAxis = x.Normalized();
    const Vector3 yAxis = y.Normalized();
    const Vector3 zAxis = z.Normalized();

    //Because we are drawing a capsule we have to have room for the "domed caps"
    const double xScale = x.Length();
    const double yScale = y.Length();
    const double zScale = z.Length();
    double capsuleRadius = radius * Math::Max(xScale, yScale);
    HalfHeight *= zScale;
    capsuleRadius = Math::Clamp(capsuleRadius, 0.0, HalfHeight);	//Cap radius based on total height
    HalfHeight -= capsuleRadius;
    HalfHeight = Math::Max(0.0, HalfHeight);

    //Draw top and bottom circles
    const Vector3 topEnd = origin + (HalfHeight * zAxis);
    const Vector3 bottomEnd = origin - (HalfHeight * zAxis);

    CreateCircle(vertices, topEnd, xAxis, yAxis, color, capsuleRadius, NumSides);
    CreateCircle(vertices, bottomEnd, xAxis, yAxis, color, capsuleRadius, NumSides);

    //Draw domed caps
    CreateHalfCircle(vertices, topEnd, yAxis, zAxis, color, capsuleRadius, NumSides / 2);
    CreateHalfCircle(vertices, topEnd, xAxis, zAxis, color, capsuleRadius, NumSides / 2);

    const Vector3 negZAxis = Vector3(-zAxis.x, -zAxis.y, -zAxis.z);

    CreateHalfCircle(vertices, bottomEnd, yAxis, negZAxis, color, capsuleRadius, NumSides / 2);
    CreateHalfCircle(vertices, bottomEnd, xAxis, negZAxis, color, capsuleRadius, NumSides / 2);

    //We set NumSides to 4 as it makes a nicer looking capsule as we only draw 2 HalfCircles above
    const int numCylinderLines = 4;

    //Draw lines for the cylinder portion 
    const float	angleDelta = 2.0f * M_PI / numCylinderLines;
    Vector3	lastVertex = base + xAxis * capsuleRadius;

    for (int sideIndex = 0; sideIndex < numCylinderLines; sideIndex++)
    {
        const Vector3 vertex = base + (xAxis * Math::Cos(angleDelta * (sideIndex + 1)) + yAxis * Math::Sin(angleDelta * (sideIndex + 1))) * capsuleRadius;

        vertices.push_back({ lastVertex - zAxis * HalfHeight, color });
        vertices.push_back({ lastVertex + zAxis * HalfHeight, color });

        lastVertex = vertex;
    }
}
