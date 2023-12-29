#pragma once

#include "Utility/UI.h"
#include "BasePanel.h"
#include "Resources/PackageList.h"

class PackageListPanel : public BasePanel
{
public:
	PackageListPanel(const char* name, const char* icon, std::shared_ptr<PackageList> packageListResource);
	void Render() override;
	void OnResourceLoaded();

private:
	std::shared_ptr<PackageList> packageListResource;
	std::vector<UI::TableColumn> tableColumns;
};
