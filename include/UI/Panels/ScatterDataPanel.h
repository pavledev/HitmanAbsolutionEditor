#pragma once

#include "Utility/UI.h"
#include "BasePanel.h"
#include "Resources/ScatterData.h"

class ScatterDataPanel : public BasePanel
{
public:
	ScatterDataPanel(const char* name, const char* icon, std::shared_ptr<ScatterData> scatterDataResource);
	void Render() override;
	void OnResourceLoaded();

private:
	std::shared_ptr<ScatterData> scatterDataResource;
	std::vector<UI::TableColumn> tableColumns;
};
