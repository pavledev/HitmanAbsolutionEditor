#pragma once

#include "Utility/UI.h"
#include "BasePanel.h"
#include "Resources/GlobalResourceIndex.h"

class GlobalResourceIndexPanel : public BasePanel
{
public:
	GlobalResourceIndexPanel(const char* name, const char* icon, std::shared_ptr<GlobalResourceIndex> globalResourceIndexResource);
	void Render() override;
	void OnResourceLoaded();

private:
	std::shared_ptr<GlobalResourceIndex> globalResourceIndexResource;
	std::vector<UI::TableColumn> tableColumns;
};
