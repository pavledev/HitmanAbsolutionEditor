#pragma once

#include "Component.h"
#include "Mesh.h"

class Collision : public Component
{
public:
	Collision(const char* name, const char* icon, std::weak_ptr<Entity> entity);
	void Initialize(std::shared_ptr<RenderPrimitive> renderPrimitive);
	void AddMesh(std::shared_ptr<Mesh> mesh);
	const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const;
	void Render() override;
	void RenderProperties() override;
	void CreateBoxMesh(NxShapeDesc* shapeDescriptor, std::shared_ptr<Mesh> boxMesh);
	void CreateSphereMesh(NxShapeDesc* shapeDescriptor, std::shared_ptr<Mesh> sphereMesh);
	void CreateCapsuleMesh(NxShapeDesc* shapeDescriptor, std::shared_ptr<Mesh> capsuleMesh);

private:
	std::vector<std::shared_ptr<Mesh>> meshes;
};
