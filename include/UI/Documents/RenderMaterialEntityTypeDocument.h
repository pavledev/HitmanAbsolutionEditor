#pragma once

#include "Document.h"
#include "Resources/RenderMaterialEntityType.h"

class RenderMaterialEntityTypeDocument : public Document
{
public:
	RenderMaterialEntityTypeDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID);
	void CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize) override;
	std::shared_ptr<RenderMaterialEntityType> GetRenderMaterialEntityType() const;
	void RenderMenuBar() override;

private:
	void OnResourceLoaded();

	std::shared_ptr<RenderMaterialEntityType> renderMaterialEntityType;
};
