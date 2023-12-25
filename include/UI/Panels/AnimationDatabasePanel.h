#pragma once

#include "Utility/UI.h"
#include "BasePanel.h"
#include "Resources/AnimationDatabase.h"

class AnimationDatabasePanel : public BasePanel
{
public:
	AnimationDatabasePanel(const char* name, const char* icon, std::shared_ptr<AnimationDatabase> animationDatabaseResource);
	void Render() override;
	void OnResourceLoaded();

private:
	std::shared_ptr<AnimationDatabase> animationDatabaseResource;
	std::vector<UI::TableColumn> tableColumns;
};
