#pragma once

#include "Document.h"
#include "Resources/RenderMaterialInstance.h"

class RenderMaterialInstanceDocument : public Document
{
public:
	RenderMaterialInstanceDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID);
	void CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize) override;
	std::shared_ptr<RenderMaterialInstance> GetRenderMaterialInstance() const;
	void RenderMenuBar() override;

private:
	std::shared_ptr<RenderMaterialInstance> renderMaterialInstance;
};
