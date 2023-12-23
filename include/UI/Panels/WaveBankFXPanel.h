#pragma once

#include "Utility/UI.h"
#include "BasePanel.h"
#include "Resources/WaveBankFX.h"

class WaveBankFXPanel : public BasePanel
{
public:
	WaveBankFXPanel(const char* name, const char* icon, std::shared_ptr<WaveBankFX> waveBankFXResource);
	void Render() override;
	void OnResourceLoaded();

private:
	std::shared_ptr<WaveBankFX> waveBankFXResource;
	std::vector<UI::TableColumn> tableColumns;
};
