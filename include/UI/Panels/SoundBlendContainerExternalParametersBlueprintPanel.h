#pragma once

#include "Utility/UI.h"
#include "BasePanel.h"
#include "Resources/SoundBlendContainerExternalParametersBlueprint.h"

class SoundBlendContainerExternalParametersBlueprintPanel : public BasePanel
{
public:
	SoundBlendContainerExternalParametersBlueprintPanel(const char* name, const char* icon, std::shared_ptr<SoundBlendContainerExternalParametersBlueprint> soundBlendContainerExternalParametersBlueprintResource);
	void Render() override;
	void OnResourceLoaded();

private:
	std::shared_ptr<SoundBlendContainerExternalParametersBlueprint> soundBlendContainerExternalParametersBlueprintResource;
	std::vector<UI::TableColumn> tableColumns;
};
