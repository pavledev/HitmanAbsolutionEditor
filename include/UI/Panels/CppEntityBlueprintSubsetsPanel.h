#pragma once

#include "Utility/UI.h"
#include "BasePanel.h"
#include "UI/Panels/CppEntityBlueprintSubsetsPanel.h"
#include "Resources/CppEntityBlueprint.h"

class CppEntityBlueprintSubsetsPanel : public BasePanel
{
public:
	CppEntityBlueprintSubsetsPanel(const char* name, const char* icon, std::shared_ptr<CppEntityBlueprint> cbluResource);
	void Render() override;
	void OnResourceLoaded();
	void SetResource(std::shared_ptr<CppEntityBlueprint> cbluResource);

private:
	void RenderSubsets();

	std::shared_ptr<CppEntityBlueprint> cbluResource;
	std::vector<UI::TableColumn> tableColumns;
};
