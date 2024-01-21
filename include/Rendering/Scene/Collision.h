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
	static void CreateBoxMesh(NxShapeDesc* shapeDescriptor, std::shared_ptr<Mesh> boxMesh);
	static void CreateSphereMesh(NxShapeDesc* shapeDescriptor, std::shared_ptr<Mesh> sphereMesh);
	static void CreateCapsuleMesh(NxShapeDesc* shapeDescriptor, std::shared_ptr<Mesh> capsuleMesh);
	static void CreateConvexMesh(NxShapeDesc* shapeDescriptor, std::shared_ptr<Mesh> convexMesh);
	static void CreateTriangleMesh(NxShapeDesc* shapeDescriptor, std::shared_ptr<Mesh> triangleMesh);

private:
	std::vector<std::shared_ptr<Mesh>> meshes;
};
