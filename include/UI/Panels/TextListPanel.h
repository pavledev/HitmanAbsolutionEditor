#pragma once

#include "Utility/UI.h"
#include "BasePanel.h"
#include "Resources/TextList.h"

class TextListPanel : public BasePanel
{
public:
	TextListPanel(const char* name, const char* icon, std::shared_ptr<TextList> textListResource);
	void Render() override;
	void OnResourceLoaded();

private:
	std::shared_ptr<TextList> textListResource;
	std::vector<UI::TableColumn> tableColumns;
};
