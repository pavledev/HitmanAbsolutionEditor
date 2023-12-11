#pragma once

#include <vector>

#include "BasePanel.h"
#include "Resources/HeaderLibrary.h"

class LibraryPanel : public BasePanel
{
public:
	LibraryPanel(const char* name, const char* icon);
	void Render() override;
	void LoadHeaderLibraryResourceIDs();

private:
	std::vector<std::string> headerLibraryResourceIDs;
	std::string currentHeaderLibraryResourceID;
	HeaderLibrary headerLibrary;
	HeaderLibrary::HeaderLibraryChunk headerLibraryChunk;
};
