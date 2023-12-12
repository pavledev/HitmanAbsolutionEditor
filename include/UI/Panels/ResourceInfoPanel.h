#pragma once

#include "BasePanel.h"
#include "Resources/Resource.h"

class ResourceInfoPanel : public BasePanel
{
public:
	ResourceInfoPanel(const char* name, const char* icon, std::shared_ptr<Resource> resource);
	void Render() override;
	void OnResourceLoaded();

private:
	void FilterReferences(const char* reference, const char* hint, const std::vector<std::shared_ptr<Resource>>& references, std::vector<std::shared_ptr<Resource>>& filteredReferences);
	void DisplayTextureInfo();

	std::shared_ptr<Resource> resource;
	char reference[256]{ "" };
	char backReference[256]{ "" };
	std::string currentHeaderLibraryResourceID;
};
