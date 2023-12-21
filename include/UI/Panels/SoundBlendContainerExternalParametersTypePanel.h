#pragma once

#include "Utility/UI.h"
#include "BasePanel.h"
#include "Resources/SoundBlendContainerExternalParametersType.h"

class SoundBlendContainerExternalParametersTypePanel : public BasePanel
{
public:
	SoundBlendContainerExternalParametersTypePanel(const char* name, const char* icon, std::shared_ptr<SoundBlendContainerExternalParametersType> soundBlendContainerExternalParametersTypeResource);
	void Render() override;
	void OnResourceLoaded();

private:
	std::shared_ptr<SoundBlendContainerExternalParametersType> soundBlendContainerExternalParametersTypeResource;
	std::vector<UI::TableColumn> tableColumns;
};
