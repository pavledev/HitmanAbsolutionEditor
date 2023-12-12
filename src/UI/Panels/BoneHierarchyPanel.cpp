#include <IconsMaterialDesignIcons.h>

#include "UI/Panels/BoneHierarchyPanel.h"
#include "Utility/StringUtility.h"

BoneHierarchyPanel::BoneHierarchyPanel(const char* name, const char* icon, std::shared_ptr<RenderPrimitive> renderPrimitive) : BasePanel(name, icon)
{
    this->renderPrimitive = renderPrimitive;
}

void BoneHierarchyPanel::Render()
{
    if (!Begin())
    {
        return;
    }

    if (renderPrimitive->GetBoneRig())
    {
        RenderTree(-1);
    }

    End();
}

void BoneHierarchyPanel::RenderTree(unsigned int parentIndex)
{
    const std::vector<SBoneDefinition>& boneDefinitions = renderPrimitive->GetBoneRig()->GetBoneDefinitions();
    std::vector<unsigned int> children;

    for (size_t i = 0; i < boneDefinitions.size(); ++i)
    {
        if (boneDefinitions[i].lPrevBoneNr == parentIndex)
        {
            children.push_back(i);
        }
    }

    std::string boneName = std::format("{} {}", ICON_MDI_BONE, boneDefinitions[parentIndex + 1].Name);
    bool isNodeOpen = ImGui::TreeNode(boneName.c_str());

    if (isNodeOpen)
    {
        for (size_t i = 0; i < children.size(); ++i)
        {
            if (children[i] < boneDefinitions.size() - 1)
            {
                RenderTree(children[i]);
            }
        }

        ImGui::TreePop();
    }
}
