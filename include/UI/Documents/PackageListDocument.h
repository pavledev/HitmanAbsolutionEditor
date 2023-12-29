#pragma once

#include "Document.h"
#include "Resources/PackageList.h"

class PackageListDocument : public Document
{
public:
	PackageListDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID);
	void CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize) override;
	std::shared_ptr<PackageList> GetPackageList() const;
	void RenderMenuBar() override;

private:
	std::shared_ptr<PackageList> packageList;
};
