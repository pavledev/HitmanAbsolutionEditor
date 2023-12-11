#pragma once

#include <DirectXMath.h>

#include "imgui.h"

#include "Component.h"

class Grid : public Component
{
public:
	Grid(const char* name, const char* icon, std::weak_ptr<Entity> entity);
	~Grid() override;
	void Initialize();
	void Render() override;
	void RenderProperties() override;

private:
	float size;
	ImDrawData drawData;
	ImDrawList* drawList;
};
