#pragma once

#include "Document.h"
#include "Resources/BehaviorTreeEntityBlueprint.h"

class BehaviorTreeEntityBlueprintDocument : public Document
{
public:
	BehaviorTreeEntityBlueprintDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID);
	void CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize) override;
	std::shared_ptr<BehaviorTreeEntityBlueprint> GetBehaviorTreeEntityBlueprint() const;
	void RenderMenuBar() override;

private:
	std::shared_ptr<BehaviorTreeEntityBlueprint> behaviorTreeEntityBlueprint;
};
