#pragma once

#include "Document.h"
#include "Resources/SoundDefinitions.h"

class SoundDefinitionsDocument : public Document
{
public:
	SoundDefinitionsDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID);
	void CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize) override;
	std::shared_ptr<SoundDefinitions> GetSoundDefinitions() const;
	void RenderMenuBar() override;

private:
	std::shared_ptr<SoundDefinitions> soundDefinitions;
};
