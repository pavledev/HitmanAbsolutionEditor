#pragma once

#include <vector>
#include <map>
#include <set>
#include <string>
#include <d3d11.h>

#include "BasePanel.h"
#include "UI/Documents/Document.h"
#include "Resources/Resource.h"
#include "Rendering/DirectXRenderer.h"

class ResourceBrowserPanel : public BasePanel
{
public:
	struct ResourceNode
	{
		unsigned int index;
		unsigned long long hash;
		std::string name;
		std::vector<ResourceNode> children;
	};

	ResourceBrowserPanel(const char* name, const char* icon);
	~ResourceBrowserPanel();
	void Render() override;
	void RenderTree(ResourceNode& parentNode, std::string parentPath);
	void RenderContextMenu(ResourceNode& resourceNode);
	void RenderFolderContextMenu(ResourceNode& folderNode, const std::string& folderPath);
	void AddChildren(ResourceNode& parentNode, const std::string& parentPath);
	void AddRootResourceNodes();
	void LoadResourceTypes();
	void LoadResource(std::shared_ptr<Resource> resource, const ResourceNode& resourceNode, const bool loadBackReferences = true);
	void CreateResourceDocument(const ResourceNode& resourceNode);

	void CollectTeliChildren(const std::string& folderPath,
		const std::vector<std::string>& languageFilters,
		std::vector<unsigned long long>& outHashes,
		std::vector<std::string>& outNames);
	void BatchExportTeliFiles(const std::string& outputFolder, const std::string& exportFormat);
	void BatchImportTeliFiles(const std::string& inputFolder);

private:
	ResourceNode assemblyNode;
	ResourceNode modulesNode;
	std::map<std::string, bool> resourceTypes;
	char resourceName[256] = { "" };
	bool isInputTextActive;
	unsigned int nodeIndex;
	unsigned int selectedNodeIndex;
	bool showResourceExportPopup;
	bool showImportJsonPopup;
	bool showPatchPopup;
	bool showFsbsPatchPopup;
	std::shared_ptr<Resource> resource;
	ResourceNode pendingResourceNode;

	// Batch TELI export/import state
	bool showBatchTeliExportPopup;
	bool showBatchTeliImportPopup;
	std::string batchTeliParentPath;
	std::vector<unsigned long long> batchTeliHashes;
	std::vector<std::string> batchTeliNames;
	std::vector<std::string> detectedLanguages;
	std::vector<bool> selectedLanguages;
	int batchExportFormatIndex;
};
