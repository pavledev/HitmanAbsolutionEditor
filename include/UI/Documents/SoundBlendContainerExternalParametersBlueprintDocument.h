#pragma once

#include "Document.h"
#include "Resources/SoundBlendContainerExternalParametersBlueprint.h"

class SoundBlendContainerExternalParametersBlueprintDocument : public Document
{
public:
	SoundBlendContainerExternalParametersBlueprintDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID);
	void CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize) override;
	std::shared_ptr<SoundBlendContainerExternalParametersBlueprint> GetSoundBlendContainerExternalParametersBlueprint() const;
	void RenderMenuBar() override;

private:
	std::shared_ptr<SoundBlendContainerExternalParametersBlueprint> soundBlendContainerExternalParametersBlueprint;
};
