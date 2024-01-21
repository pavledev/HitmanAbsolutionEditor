#pragma once

#include "Component.h"
#include "BulletCollision.h"

class CollisionBone : public Component
{
public:
	CollisionBone(const char* name, const char* icon, std::weak_ptr<Entity> entity);
	void Initialize(std::shared_ptr<RenderPrimitive::Mesh> mesh, std::shared_ptr<Entity> collisionBoneEntity, std::shared_ptr<Renderer3D> renderer3D);
	void Render() override;
	void RenderProperties() override;

private:
	std::vector<std::shared_ptr<BulletCollision>> bulletCollisions;
};
