#pragma once

#include "Document.h"

class HeaderLibrariesSearchDocument : public Document
{
public:
	HeaderLibrariesSearchDocument(const char* name, const char* icon, const Type type, const ImGuiID dockID);
	void CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize) override;
};
