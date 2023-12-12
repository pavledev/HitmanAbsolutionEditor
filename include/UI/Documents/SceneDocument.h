#pragma once

#include "Document.h"

class SceneDocument : public Document
{
public:
	SceneDocument(const char* name, const char* icon, const Type type);
	void CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize) override;
	void RenderMenuBar() override;
};
