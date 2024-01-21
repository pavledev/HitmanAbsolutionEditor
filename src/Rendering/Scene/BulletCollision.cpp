#include <IconsMaterialDesignIcons.h>

#include "Rendering/Scene/BulletCollision.h"
#include "Rendering/Scene/Geometry.h"
#include "Rendering/Scene/Transform.h"

BulletCollision::BulletCollision(const char* name, const char* icon, std::weak_ptr<Entity> entity) : Component(name, icon, entity)
{
}

void BulletCollision::Initialize(const RenderPrimitive::CollisionBox& collisionBox, const Vector3& boundingBoxMin, const Vector3& boundingBoxMax, std::shared_ptr<Entity> bulletCollisionEntity, std::shared_ptr<Renderer3D> renderer3D)
{
    for (size_t i = 0; i < collisionBox.chunks.size(); ++i)
    {
        const float x = (static_cast<float>(collisionBox.chunks[i].min[0]) / 255) * (boundingBoxMax[0] - boundingBoxMin[0]);
        const float y = (static_cast<float>(collisionBox.chunks[i].min[1]) / 255) * (boundingBoxMax[1] - boundingBoxMin[1]);
        const float z = (static_cast<float>(collisionBox.chunks[i].min[2]) / 255) * (boundingBoxMax[2] - boundingBoxMin[2]);

        const float x1 = (static_cast<float>(collisionBox.chunks[i].max[0]) / 255) * (boundingBoxMax[0] - boundingBoxMin[0]);
        const float y1 = (static_cast<float>(collisionBox.chunks[i].max[1]) / 255) * (boundingBoxMax[1] - boundingBoxMin[1]);
        const float z1 = (static_cast<float>(collisionBox.chunks[i].max[2]) / 255) * (boundingBoxMax[2] - boundingBoxMin[2]);

        const float positionX = (x1 + x) / 2 + boundingBoxMin[0];
        const float positionY = (y1 + y) / 2 + boundingBoxMin[1];
        const float positionZ = (z1 + z) / 2 + boundingBoxMin[2];

        const float scaleX = (x1 - x) / 2;
        const float scaleY = (y1 - y) / 2;
        const float scaleZ = (z1 - z) / 2;

        std::string meshName = std::format("{} Box Shape {}", ICON_MDI_SHAPE, i + 1);
        std::shared_ptr<Entity> meshEntity = std::make_shared<Entity>(meshName);

        meshEntity->Initialize();
        meshEntity->AddComponent<Mesh>("Mesh", ICON_MDI_SHAPE);
        meshEntity->GetComponent<Mesh>()->SetRenderer3D(renderer3D);
        bulletCollisionEntity->AddChild(meshEntity);

        meshes.push_back(meshEntity->GetComponent<Mesh>());
        CreateBoxMesh(Vector3(positionX, positionY, positionZ), Vector3(scaleX, scaleY, scaleZ), meshes[i]);

        meshes[i]->GetTransform()->SetParent(GetTransform());

        GetTransform()->AddChild(meshes[i]->GetTransform());
    }
}

void BulletCollision::Render()
{
}

void BulletCollision::RenderProperties()
{
}

void BulletCollision::CreateBoxMesh(const Vector3& position, const Vector3& scale, std::shared_ptr<Mesh> boxMesh)
{
    std::vector<VertexPosition> cubeVertices;
    std::vector<unsigned short> cubeIndices;

    if (cubeVertices.empty())
    {
        Geometry::CreateCube(cubeVertices, cubeIndices);
    }

    boxMesh->Initialize(cubeVertices, cubeIndices, Renderer3D::Shaders::SimpleVertex, Renderer3D::Shaders::SimplePixel, Vector4(1.f, 0.f, 0.f, 1.f), PrimitiveType::LineList);

    boxMesh->GetTransform()->SetLocalPosition(position);
    boxMesh->GetTransform()->SetLocalScale(scale);
}
