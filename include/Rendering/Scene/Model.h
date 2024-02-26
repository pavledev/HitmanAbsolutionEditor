#pragma once

#include <set>

#include "Component.h"
#include "Mesh.h"
#include "Skeleton.h"
#include "Collision.h"

class Cloth;

class Model : public Component
{
public:
	Model(const char* name, const char* icon, std::weak_ptr<Entity> entity);
	void Initialize(std::shared_ptr<RenderPrimitive> renderPrimitive);
	void Initialize(std::shared_ptr<Cloth> cloth);
	const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const;
	void AddMesh(std::shared_ptr<Mesh> mesh);
	void SetSkeleton(std::shared_ptr<Skeleton> skeleton);
	void SetCollision(std::shared_ptr<Collision> collision);
	void Render() override;
	void RenderProperties() override;
	void UpdateLODVisibility();

private:
	std::vector<std::shared_ptr<Mesh>> meshes;
	bool wireframe;
	unsigned int lod;
	std::set<unsigned char> lods;
	std::vector<std::string> names;
	std::vector<bool> flags;
	bool renderBones;
	std::shared_ptr<Skeleton> skeleton;
	std::shared_ptr<Collision> collision;
};
