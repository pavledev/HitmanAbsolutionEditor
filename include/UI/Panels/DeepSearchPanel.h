#pragma once

#include <map>
#include <vector>
#include <set>

#include "BasePanel.h"

class DeepSearchPanel : public BasePanel
{
public:
	struct TreeNode
	{
		unsigned int index;
		std::string name;
		std::vector<TreeNode> children;
	};

	HeaderLibrariesSearchPanel(const char* name, const char* icon);
	void Render() override;
	void RenderTree(TreeNode& treeNode);
	void LoadHeaderLibraryResourceIDs();
	void LoadResourceTypes();
	void FilterHeaderLibraryResourceIDs(const char* hint, std::map<std::string, bool*>& filteredHeaderLibraryResourceIDs);
	void SearchHeaderLibraries(std::map<std::string, bool*>& filteredHeaderLibraryResourceIDs);

private:
	std::map<std::string, bool> headerLibraryResourceIDs;
	std::map<std::string, bool> resourceTypes;
	char headerLibraryResourceID[512]{ "" };
	char searchText[512]{ "" };
	std::vector<TreeNode> treeNodes;
	std::set<unsigned int> selectedNodeIndices;
	unsigned int selectedNodeIndex;
};
