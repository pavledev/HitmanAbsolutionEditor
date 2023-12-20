#pragma once

#include "Utility/UI.h"
#include "BasePanel.h"
#include "Resources/MultiLanguage.h"

class MultiLanguagePanel : public BasePanel
{
public:
	MultiLanguagePanel(const char* name, const char* icon, std::shared_ptr<MultiLanguage> multiLanguageResource);
	void Render() override;
	void OnResourceLoaded();

private:
	std::shared_ptr<MultiLanguage> multiLanguageResource;
	std::vector<UI::TableColumn> tableColumns;
};
