#include <IconsMaterialDesignIcons.h>

#include "Rendering/Scene/CollisionBone.h"
#include "Rendering/Scene/Geometry.h"
#include "Rendering/Scene/Transform.h"

CollisionBone::CollisionBone(const char* name, const char* icon, std::weak_ptr<Entity> entity) : Component(name, icon, entity)
{
}

void CollisionBone::Initialize(std::shared_ptr<RenderPrimitive::Mesh> mesh, std::shared_ptr<Entity> collisionBoneEntity, std::shared_ptr<Renderer3D> renderer3D)
{
    const SPrimObject::SUBTYPE subType = mesh->GetSubType();

    if (subType == SPrimObject::SUBTYPE::SUBTYPE_LINKED)
    {
        const std::shared_ptr<RenderPrimitive::LinkedMesh> linkedMesh = std::static_pointer_cast<RenderPrimitive::LinkedMesh>(mesh);
        const std::vector<RenderPrimitive::CollisionBox>& collisionBoxes = linkedMesh->GetCollisionBoxes();

        for (size_t i = 0; i < collisionBoxes.size(); ++i)
        {
            const RenderPrimitive::CollisionBox& collisionBox = collisionBoxes[i];
            const Vector3 boundingBoxMin = linkedMesh->GetBoundingBoxMin();
            const Vector3 boundingBoxMax = linkedMesh->GetBoundingBoxMax();

            std::string bulletCollisionEntityName = std::format("{} Bullet Collision", ICON_MDI_SHAPE, i + 1);
            std::shared_ptr<Entity> bulletCollisionEntity = std::make_shared<Entity>(bulletCollisionEntityName);

            bulletCollisionEntity->Initialize();
            bulletCollisionEntity->AddComponent<BulletCollision>("Bullet Collision", ICON_MDI_SHAPE);
            bulletCollisionEntity->GetComponent<BulletCollision>()->Initialize(collisionBox, boundingBoxMin, boundingBoxMax, bulletCollisionEntity, renderer3D);

            collisionBoneEntity->AddChild(bulletCollisionEntity);
            bulletCollisionEntity->GetComponent<BulletCollision>()->GetTransform()->SetParent(collisionBoneEntity->GetComponent<Transform>());
            collisionBoneEntity->GetComponent<Transform>()->AddChild(bulletCollisionEntity->GetComponent<BulletCollision>()->GetTransform());
        }
    }
}

void CollisionBone::Render()
{
}

void CollisionBone::RenderProperties()
{
}
