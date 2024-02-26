#pragma once

#include "Document.h"
#include "Resources/BoneRig.h"

class BoneRigDocument : public Document
{
public:
	BoneRigDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID);
	void CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize) override;
	std::shared_ptr<BoneRig> GetBoneRig() const;
	void RenderMenuBar() override;

private:
	void OnResourceLoaded();

	std::shared_ptr<BoneRig> boneRig;
};
