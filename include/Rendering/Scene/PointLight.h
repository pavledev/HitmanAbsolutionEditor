#pragma once

#include "Component.h"
#include "SolidSphere.h"
#include "Mesh.h"

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

private:
	std::shared_ptr<Mesh> mesh;
	float diffuseIntensity;
	Vector3 diffuseColor;
	Vector3 ambientColor;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
};
