#pragma once

#include "Utility/UI.h"
#include "BasePanel.h"
#include "Resources/BehaviorTreeEntityBlueprint.h"

class BehaviorTreeEntityBlueprintPanel : public BasePanel
{
public:
	BehaviorTreeEntityBlueprintPanel(const char* name, const char* icon, std::shared_ptr<BehaviorTreeEntityBlueprint> behaviorTreeEntityBlueprintResource);
	void Render() override;
	void OnResourceLoaded();

private:
	std::shared_ptr<BehaviorTreeEntityBlueprint> behaviorTreeEntityBlueprintResource;
	std::vector<UI::TableColumn> tableColumns;
};
