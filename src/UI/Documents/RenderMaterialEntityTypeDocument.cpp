#include <thread>

#include <IconsMaterialDesignIcons.h>

#include <UI/Documents/RenderMaterialEntityTypeDocument.h>
#include <UI/Panels/RenderMaterialEntityTypePanel.h>
#include <UI/Panels/RenderMaterialInstancePanel.h>
#include <UI/Panels/ResourceInfoPanel.h>
#include <UI/Panels/HexViewerPanel.h>
#include <Editor.h>
#include <Registry/ResourceInfoRegistry.h>
#include <Utility/ResourceUtility.h>

RenderMaterialEntityTypeDocument::RenderMaterialEntityTypeDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID) : Document(name, icon, type, runtimeResourceID, hasToolBar, dockID)
{
    renderMaterialEntityType = std::make_shared<RenderMaterialEntityType>();

    std::shared_ptr<RenderMaterialEntityTypePanel> renderMaterialEntityTypePanel = std::make_shared<RenderMaterialEntityTypePanel>("Render Material Entity Type", ICON_MDI_PALETTE, renderMaterialEntityType);
    std::shared_ptr<RenderMaterialInstancePanel> renderMaterialInstancePanel = std::make_shared<RenderMaterialInstancePanel>("Render Material Instance", ICON_MDI_PALETTE, nullptr);
    std::shared_ptr<ResourceInfoPanel> resourceInfoPanel = std::make_shared<ResourceInfoPanel>("Resource Info", ICON_MDI_INFORMATION, renderMaterialEntityType);
    std::shared_ptr<HexViewerPanel> headerLibraryHexViewerPanel = std::make_shared<HexViewerPanel>("Header Library Hex Viewer", ICON_MDI_MEMORY, false, renderMaterialEntityType);
    std::shared_ptr<HexViewerPanel> resourceLibraryhexViewerPanel = std::make_shared<HexViewerPanel>("Resource Library Hex Viewer", ICON_MDI_MEMORY, true, renderMaterialEntityType);

    AddPanel(renderMaterialEntityTypePanel);
    AddPanel(renderMaterialInstancePanel);
    AddPanel(resourceInfoPanel);
    AddPanel(headerLibraryHexViewerPanel);
    AddPanel(resourceLibraryhexViewerPanel);

    std::weak_ptr<RenderMaterialEntityTypePanel> renderMaterialEntityTypePanel2 = renderMaterialEntityTypePanel;
    std::weak_ptr<ResourceInfoPanel> resourceInfoPanel2 = resourceInfoPanel;

    renderMaterialEntityType->SetResourceLoadedCallback([renderMaterialEntityTypePanel2, resourceInfoPanel2, this]()
    {
        std::shared_ptr<RenderMaterialEntityTypePanel> renderMaterialEntityTypePanel3 = renderMaterialEntityTypePanel2.lock();
        std::shared_ptr<ResourceInfoPanel> resourceInfoPanel3 = resourceInfoPanel2.lock();

        if (renderMaterialEntityTypePanel3)
        {
            renderMaterialEntityTypePanel3->OnResourceLoaded();
        }

        if (resourceInfoPanel3)
        {
            resourceInfoPanel3->OnResourceLoaded();
        }

        OnResourceLoaded();
    });
}

void RenderMaterialEntityTypeDocument::CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize)
{
    ImGuiID mainDockID = dockspaceID;
    ImGuiID rightDockID = ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Right, 0.5f, nullptr, &mainDockID);

    ImGui::DockBuilderDockWindow(CalculatePanelID(0, currentDockspaceID).c_str(), mainDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(1, currentDockspaceID).c_str(), mainDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(2, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(3, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(4, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderFinish(dockspaceID);
}

std::shared_ptr<RenderMaterialEntityType> RenderMaterialEntityTypeDocument::GetRenderMaterialEntityType() const
{
    return renderMaterialEntityType;
}

void RenderMaterialEntityTypeDocument::RenderMenuBar()
{
    if (!ImGui::BeginMenuBar())
        return;

    if (ImGui::BeginMenu("View"))
    {
        for (size_t i = 0; i < panels.size(); ++i)
        {
            ImGui::MenuItem(panels[i]->GetName(), nullptr, panels[i]->GetOpen());
        }

        ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
}

void RenderMaterialEntityTypeDocument::OnResourceLoaded()
{
    std::vector<std::shared_ptr<Resource>>& mattReferences = renderMaterialEntityType->GetReferences();
    std::shared_ptr<RenderMaterialInstance> renderMaterialInstance;

    for (size_t i = 0; i < mattReferences.size(); ++i)
    {
        const ResourceInfoRegistry::ResourceInfo& referenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(mattReferences[i]->GetHash());

        if (referenceInfo.type == "MATI")
        {
            renderMaterialInstance = std::static_pointer_cast<RenderMaterialInstance>(mattReferences[i]);

            break;
        }
    }

    const ResourceInfoRegistry::ResourceInfo& matiResourceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(renderMaterialInstance->GetHash());

    renderMaterialInstance->LoadResource(0, matiResourceInfo.headerLibraries[0].chunkIndex, matiResourceInfo.headerLibraries[0].indexInLibrary, false, false, true);
    renderMaterialInstance->Deserialize();
    renderMaterialInstance->DeleteResourceData();

    std::shared_ptr<RenderMaterialInstancePanel> renderMaterialInstancePanel = std::static_pointer_cast<RenderMaterialInstancePanel>(panels[1]);
    std::weak_ptr<RenderMaterialInstancePanel> renderMaterialInstancePanel2 = renderMaterialInstancePanel;

    renderMaterialInstancePanel->SetResource(renderMaterialInstance);

    renderMaterialInstance->SetResourceLoadedCallback([renderMaterialInstancePanel2]()
    {
        std::shared_ptr<RenderMaterialInstancePanel> renderMaterialInstancePanel3 = renderMaterialInstancePanel2.lock();

        if (renderMaterialInstancePanel3)
        {
            renderMaterialInstancePanel3->OnResourceLoaded();
        }
    });

    std::thread thread(&ResourceUtility::LoadResource, renderMaterialInstance);

    thread.detach();
}
