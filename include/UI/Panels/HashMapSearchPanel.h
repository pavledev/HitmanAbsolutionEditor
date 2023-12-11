#pragma once

#include <unordered_map>
#include <vector>
#include <set>

#include "BasePanel.h"

class HashMapSearchPanel : public BasePanel
{
public:
	HashMapSearchPanel(const char* name, const char* icon);
	void Render() override;
	void AddResourceIDs();
	void FilterHashMap(const char* hint, std::unordered_map<const char*, const char*>& filteredHeaderLibraryResourceIDs);

private:
	std::unordered_map<std::string, const char*> resourceIDs;
	char searchText[512]{ "" };
};
