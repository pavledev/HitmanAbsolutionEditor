#pragma once

#include "Component.h"
#include "Math/Vector4.h"

class Renderer3D;

class DirectionalLight : public Component
{
public:
	DirectionalLight() = default;
	DirectionalLight(const char* name, const char* icon, std::weak_ptr<Entity> entity);
	void Render() override;
	void RenderProperties() override;
	void SetRenderer3D(std::shared_ptr<Renderer3D> renderer3D);

private:
	Vector3 lightDirection;
	Vector4 lightColor;
	std::shared_ptr<Renderer3D> renderer3D;
};
