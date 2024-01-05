#include <IconsMaterialDesignIcons.h>

#include <UI/Documents/CppEntityBlueprintDocument.h>
#include <UI/Panels/CppEntityBlueprintSubsetsPanel.h>
#include <UI/Panels/ResourceInfoPanel.h>
#include <UI/Panels/HexViewerPanel.h>
#include <UI/Documents/AnimationDatabaseDocument.h>

CppEntityBlueprintDocument::CppEntityBlueprintDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID) : Document(name, icon, type, runtimeResourceID, hasToolBar, dockID)
{
    cppEntityBlueprint = std::make_shared<CppEntityBlueprint>();

    std::shared_ptr<CppEntityBlueprintSubsetsPanel> cppEntityBlueprintSubsetsPanel = std::make_shared<CppEntityBlueprintSubsetsPanel>("Subsets", ICON_MDI_WRENCH, cppEntityBlueprint);
    std::shared_ptr<ResourceInfoPanel> resourceInfoPanel = std::make_shared<ResourceInfoPanel>("Resource Info", ICON_MDI_INFORMATION, cppEntityBlueprint);
    std::shared_ptr<HexViewerPanel> headerLibraryHexViewerPanel = std::make_shared<HexViewerPanel>("Header Library Hex Viewer", ICON_MDI_MEMORY, false, cppEntityBlueprint);
    std::shared_ptr<HexViewerPanel> resourceLibraryhexViewerPanel = std::make_shared<HexViewerPanel>("Resource Library Hex Viewer", ICON_MDI_MEMORY, true, cppEntityBlueprint);

    AddPanel(cppEntityBlueprintSubsetsPanel);
    AddPanel(resourceInfoPanel);
    AddPanel(headerLibraryHexViewerPanel);
    AddPanel(resourceLibraryhexViewerPanel);

    std::weak_ptr<CppEntityBlueprintSubsetsPanel> cppEntityBlueprintSubsetsPanel2 = cppEntityBlueprintSubsetsPanel;
    std::weak_ptr<ResourceInfoPanel> resourceInfoPanel2 = resourceInfoPanel;

    cppEntityBlueprint->SetResourceLoadedCallback([cppEntityBlueprintSubsetsPanel2, resourceInfoPanel2]()
    {
        std::shared_ptr<CppEntityBlueprintSubsetsPanel> cppEntityBlueprintSubsetsPanel3 = cppEntityBlueprintSubsetsPanel2.lock();
        std::shared_ptr<ResourceInfoPanel> resourceInfoPanel3 = resourceInfoPanel2.lock();

        if (cppEntityBlueprintSubsetsPanel3)
        {
            cppEntityBlueprintSubsetsPanel3->OnResourceLoaded();
        }

        if (resourceInfoPanel3)
        {
            resourceInfoPanel3->OnResourceLoaded();
        }
    });
}

void CppEntityBlueprintDocument::CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize)
{
    ImGuiID mainDockID = dockspaceID;
    ImGuiID rightDockID = ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Right, 0.6f, nullptr, &mainDockID);

    ImGui::DockBuilderDockWindow(CalculatePanelID(0, currentDockspaceID).c_str(), mainDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(1, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(2, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(3, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderFinish(dockspaceID);
}

std::shared_ptr<CppEntityBlueprint> CppEntityBlueprintDocument::GetCppEntityBlueprint() const
{
    return cppEntityBlueprint;
}

void CppEntityBlueprintDocument::RenderMenuBar()
{
    if (!ImGui::BeginMenuBar())
    {
        return;
    }

    if (ImGui::BeginMenu("View"))
    {
        for (size_t i = 0; i < panels.size(); ++i)
        {
            ImGui::MenuItem(panels[i]->GetName(), nullptr, panels[i]->GetOpen());
        }

        ImGui::EndMenu();
    }

    static std::string exportResourceLabel = std::format("{} Export Resource", ICON_MDI_EXPORT);
    static bool showResourceExportPopup = false;

    if (ImGui::BeginMenu("Export"))
    {
        if (ImGui::MenuItem(exportResourceLabel.c_str()))
        {
            showResourceExportPopup = true;
        }

        ImGui::EndMenu();
    }

    UI::ResourceExportPopup(showResourceExportPopup, cppEntityBlueprint);

    ImGui::EndMenuBar();
}
