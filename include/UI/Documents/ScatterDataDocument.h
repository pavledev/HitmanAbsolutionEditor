#pragma once

#include "Document.h"
#include "Resources/ScatterData.h"

class ScatterDataDocument : public Document
{
public:
	ScatterDataDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID);
	void CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize) override;
	std::shared_ptr<ScatterData> GetScatterData() const;
	void RenderMenuBar() override;

private:
	std::shared_ptr<ScatterData> scatterData;
};
