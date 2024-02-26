#pragma once

#include "Document.h"
#include "Resources/Physics.h"

class PhysicsDocument : public Document
{
public:
	PhysicsDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID);
	void CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize) override;
	std::shared_ptr<Physics> GetPhysics() const;
	void RenderMenuBar() override;

private:
	void OnResourceLoaded();

	std::shared_ptr<Physics> physics;
};
