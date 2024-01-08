#pragma once

#include <DirectXMath.h>

#include "imgui.h"

#include "Component.h"

class Renderer3D;

class Grid : public Component
{
public:
	Grid(const char* name, const char* icon, std::weak_ptr<Entity> entity);
	void Initialize();
	void Render() override;
	void RenderProperties() override;
	void SetRenderer3D(std::shared_ptr<Renderer3D> renderer3D);

private:
	float size;
	std::unique_ptr<ImDrawList> drawList;
	ImDrawData drawData;
	std::shared_ptr<Renderer3D> renderer3D;
};
