#pragma once

#include "BasePanel.h"
#include "Resources/RenderPrimitive.h"
#include "Rendering/Scene/Entity.h"

class SceneHierarchyPanel2 : public BasePanel
{
public:
	SceneHierarchyPanel2(const char* name, const char* icon, std::shared_ptr<RenderPrimitive> renderPrimitive);
	static std::shared_ptr<Entity> GetRootEntity();
	static void SetSelectedEntity(std::shared_ptr<Entity> entity);
	void CreateEntities();
	void Render() override;
	void RenderTree(std::shared_ptr<Entity> parentEntity);

private:
	std::shared_ptr<RenderPrimitive> renderPrimitive;
	inline static std::shared_ptr<Entity> rootEntity;
	std::shared_ptr<Entity> hoveredEntity;
	inline static std::shared_ptr<Entity> selectedEntity;
};
