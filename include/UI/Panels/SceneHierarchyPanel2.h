#pragma once

#include "BasePanel.h"
#include "Rendering/Scene/Entity.h"
#include "Rendering/Renderer3D.h"

class SceneHierarchyPanel2 : public BasePanel
{
public:
	SceneHierarchyPanel2(const char* name, const char* icon, std::shared_ptr<Resource> resource);
	void OnSelectedEntityTreeNode(std::shared_ptr<Entity> selectedEntity);
	void CreateEntities();
	void Render() override;
	void RenderTree(std::shared_ptr<Entity> parentEntity);
	void OnResourceLoaded();
	void SetRenderer3D(std::shared_ptr<Renderer3D> renderer3D);

private:
	std::shared_ptr<Resource> resource;
	std::shared_ptr<Entity> rootEntity;
	std::shared_ptr<Entity> hoveredEntity;
	std::shared_ptr<Entity> selectedEntity;
	std::shared_ptr<Renderer3D> renderer3D;
	bool areEntitiesCreated;
};
