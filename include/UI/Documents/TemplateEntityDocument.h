#pragma once

#include "Document.h"
#include "Resources/TemplateEntity.h"

class TemplateEntityDocument : public Document
{
public:
	TemplateEntityDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID);
	void CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize) override;
	std::shared_ptr<TemplateEntity> GetTemplateEntity() const;
	void RenderMenuBar() override;

private:
	std::shared_ptr<TemplateEntity> templateEntity;
};
