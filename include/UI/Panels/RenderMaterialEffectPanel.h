#pragma once

#include "Utility/UI.h"
#include "BasePanel.h"
#include "Resources/RenderMaterialEffect.h"

class RenderMaterialEffectPanel : public BasePanel
{
public:
	RenderMaterialEffectPanel(const char* name, const char* icon, std::shared_ptr<RenderMaterialEffect> renderMaterialEffectResource);
	void Render() override;
	void OnResourceLoaded();

private:
	std::shared_ptr<RenderMaterialEffect> renderMaterialEffectResource;
	std::vector<UI::TableColumn> tableColumns;
};
