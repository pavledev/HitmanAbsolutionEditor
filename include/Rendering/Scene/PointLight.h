#pragma once

#include "Component.h"
#include "SolidSphere.h"
#include "Mesh.h"

class Renderer3D;

class PointLight : public Component
{
public:
	PointLight() = default;
	PointLight(const char* name, const char* icon, std::weak_ptr<Entity> entity);
	~PointLight() override;
	void Initialize();
	void SetMesh(std::shared_ptr<Mesh> mesh);
	void Render() override;
	void RenderProperties() override;
	void SetRenderer3D(std::shared_ptr<Renderer3D> renderer3D);

private:
	std::shared_ptr<Mesh> mesh;
	float diffuseIntensity;
	Vector3 diffuseColor;
	Vector3 ambientColor;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
	std::shared_ptr<Renderer3D> renderer3D;
};
