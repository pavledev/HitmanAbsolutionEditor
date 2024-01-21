#pragma once

#include "Component.h"
#include "Mesh.h"

class BulletCollision : public Component
{
public:
	BulletCollision(const char* name, const char* icon, std::weak_ptr<Entity> entity);
	void Initialize(const RenderPrimitive::CollisionBox& collisionBox, const Vector3& boundingBoxMin, const Vector3& boundingBoxMax, std::shared_ptr<Entity> bulletCollisionEntity, std::shared_ptr<Renderer3D> renderer3D);
	void Render() override;
	void RenderProperties() override;
	void CreateBoxMesh(const Vector3& position, const Vector3& scale, std::shared_ptr<Mesh> boxMesh);

private:
	std::vector<std::shared_ptr<Mesh>> meshes;
};
