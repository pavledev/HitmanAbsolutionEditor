#pragma once

#include "Utility/UI.h"
#include "BasePanel.h"
#include "Resources/Localization.h"

class LocalizationPanel : public BasePanel
{
public:
	LocalizationPanel(const char* name, const char* icon, std::shared_ptr<Localization> localizationResource);
	void Render() override;
	void OnResourceLoaded();

private:
	std::shared_ptr<Localization> localizationResource;
	std::vector<UI::TableColumn> tableColumns;
};
