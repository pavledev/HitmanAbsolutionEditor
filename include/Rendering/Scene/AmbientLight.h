#pragma once

#include "Component.h"
#include "Math/Vector4.h"

class Renderer3D;

class AmbientLight : public Component
{
public:
	AmbientLight() = default;
	AmbientLight(const char* name, const char* icon, std::weak_ptr<Entity> entity);
	void Render() override;
	void RenderProperties() override;
	void SetRenderer3D(std::shared_ptr<Renderer3D> renderer3D);

private:
	Vector4 lightAmbient;
	std::shared_ptr<Renderer3D> renderer3D;
};
