#include <thread>

#include <IconsMaterialDesignIcons.h>

#include <UI/Documents/CppEntityDocument.h>
#include <UI/Panels/CppEntityPropertiesPanel.h>
#include <UI/Panels/CppEntityBlueprintSubsetsPanel.h>
#include <UI/Panels/ResourceInfoPanel.h>
#include <UI/Panels/HexViewerPanel.h>
#include <Resources/CppEntityBlueprint.h>
#include <Utility/ResourceUtility.h>

CppEntityDocument::CppEntityDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID) : Document(name, icon, type, runtimeResourceID, hasToolBar, dockID)
{
    cppEntity = std::make_shared<CppEntity>();

    std::shared_ptr<CppEntityPropertiesPanel> cppEntityPropertiesPanel = std::make_shared<CppEntityPropertiesPanel>("Entity Properties", ICON_MDI_WRENCH, cppEntity);
    std::shared_ptr<CppEntityBlueprintSubsetsPanel> cppEntityBlueprintSubsetsPanel = std::make_shared<CppEntityBlueprintSubsetsPanel>("Subsets", ICON_MDI_WRENCH, nullptr);
    std::shared_ptr<ResourceInfoPanel> resourceInfoPanel = std::make_shared<ResourceInfoPanel>("Resource Info", ICON_MDI_INFORMATION, cppEntity);
    std::shared_ptr<HexViewerPanel> headerLibraryHexViewerPanel = std::make_shared<HexViewerPanel>("Header Library Hex Viewer", ICON_MDI_MEMORY, false, cppEntity);
    std::shared_ptr<HexViewerPanel> resourceLibraryhexViewerPanel = std::make_shared<HexViewerPanel>("Resource Library Hex Viewer", ICON_MDI_MEMORY, true, cppEntity);

    cppEntityBlueprintSubsetsPanel->SetOpen(false);

    AddPanel(cppEntityPropertiesPanel);
    AddPanel(cppEntityBlueprintSubsetsPanel);
    AddPanel(resourceInfoPanel);
    AddPanel(headerLibraryHexViewerPanel);
    AddPanel(resourceLibraryhexViewerPanel);

    std::weak_ptr<CppEntityPropertiesPanel> cppEntityPropertiesPanel2 = cppEntityPropertiesPanel;
    std::weak_ptr<ResourceInfoPanel> resourceInfoPanel2 = resourceInfoPanel;

    cppEntity->SetResourceLoadedCallback([cppEntityPropertiesPanel2, resourceInfoPanel2, this]()
    {
        std::shared_ptr<CppEntityPropertiesPanel> cppEntityPropertiesPanel3 = cppEntityPropertiesPanel2.lock();
        std::shared_ptr<ResourceInfoPanel> resourceInfoPanel3 = resourceInfoPanel2.lock();

        if (cppEntityPropertiesPanel3)
        {
            cppEntityPropertiesPanel3->OnResourceLoaded();
        }

        if (resourceInfoPanel3)
        {
            resourceInfoPanel3->OnResourceLoaded();
        }

        OnResourceLoaded();
    });
}

void CppEntityDocument::CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize)
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

std::shared_ptr<CppEntity> CppEntityDocument::GetCppEntity() const
{
    return cppEntity;
}

void CppEntityDocument::RenderMenuBar()
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

void CppEntityDocument::OnResourceLoaded()
{
    std::vector<std::shared_ptr<Resource>>& cpptReferences = cppEntity->GetReferences();
    std::shared_ptr<CppEntityBlueprint> cppEntityBlueprint = std::static_pointer_cast<CppEntityBlueprint>(cpptReferences[cpptReferences.size() - 1]);

    std::shared_ptr<CppEntityBlueprintSubsetsPanel> cppEntityBlueprintSubsetsPanel = std::static_pointer_cast<CppEntityBlueprintSubsetsPanel>(panels[1]);
    std::weak_ptr<CppEntityBlueprintSubsetsPanel> cppEntityBlueprintSubsetsPanel2 = cppEntityBlueprintSubsetsPanel;

    cppEntityBlueprintSubsetsPanel->SetResource(cppEntityBlueprint);

    cppEntityBlueprint->SetResourceLoadedCallback([cppEntityBlueprintSubsetsPanel2]()
    {
        std::shared_ptr<CppEntityBlueprintSubsetsPanel> cppEntityBlueprintSubsetsPanel3 = cppEntityBlueprintSubsetsPanel2.lock();

        if (cppEntityBlueprintSubsetsPanel3)
        {
            cppEntityBlueprintSubsetsPanel3->OnResourceLoaded();
        }
    });

    std::thread thread(&ResourceUtility::LoadResource, cppEntityBlueprint);

    thread.detach();
}
