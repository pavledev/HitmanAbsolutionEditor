#include <IconsMaterialDesignIcons.h>

#include <UI/Documents/BehaviorTreeEntityBlueprintDocument.h>
#include <UI/Panels/BehaviorTreeEntityBlueprintPanel.h>
#include <UI/Panels/ResourceInfoPanel.h>
#include <UI/Panels/HexViewerPanel.h>
#include <Editor.h>

BehaviorTreeEntityBlueprintDocument::BehaviorTreeEntityBlueprintDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID) : Document(name, icon, type, runtimeResourceID, hasToolBar, dockID)
{
    behaviorTreeEntityBlueprint = std::make_shared<BehaviorTreeEntityBlueprint>();

    std::shared_ptr<BehaviorTreeEntityBlueprintPanel> behaviorTreeEntityBlueprintPanel = std::make_shared<BehaviorTreeEntityBlueprintPanel>("Behavior Tree Entity Blueprint", ICON_MDI_VIEW_LIST, behaviorTreeEntityBlueprint);
    std::shared_ptr<ResourceInfoPanel> resourceInfoPanel = std::make_shared<ResourceInfoPanel>("Resource Info", ICON_MDI_INFORMATION, behaviorTreeEntityBlueprint);
    std::shared_ptr<HexViewerPanel> headerLibraryHexViewerPanel = std::make_shared<HexViewerPanel>("Header Library Hex Viewer", ICON_MDI_MEMORY, false, behaviorTreeEntityBlueprint);
    std::shared_ptr<HexViewerPanel> resourceLibraryhexViewerPanel = std::make_shared<HexViewerPanel>("Resource Library Hex Viewer", ICON_MDI_MEMORY, true, behaviorTreeEntityBlueprint);

    AddPanel(behaviorTreeEntityBlueprintPanel);
    AddPanel(resourceInfoPanel);
    AddPanel(headerLibraryHexViewerPanel);
    AddPanel(resourceLibraryhexViewerPanel);

    std::weak_ptr<BehaviorTreeEntityBlueprintPanel> behaviorTreeEntityBlueprintPanel2 = behaviorTreeEntityBlueprintPanel;
    std::weak_ptr<ResourceInfoPanel> resourceInfoPanel2 = resourceInfoPanel;

    behaviorTreeEntityBlueprint->SetResourceLoadedCallback([behaviorTreeEntityBlueprintPanel2, resourceInfoPanel2]()
    {
        std::shared_ptr<BehaviorTreeEntityBlueprintPanel> behaviorTreeEntityBlueprintPanel3 = behaviorTreeEntityBlueprintPanel2.lock();
        std::shared_ptr<ResourceInfoPanel> resourceInfoPanel3 = resourceInfoPanel2.lock();

        if (behaviorTreeEntityBlueprintPanel3)
        {
            behaviorTreeEntityBlueprintPanel3->OnResourceLoaded();
        }

        if (resourceInfoPanel3)
        {
            resourceInfoPanel3->OnResourceLoaded();
        }
    });
}

void BehaviorTreeEntityBlueprintDocument::CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize)
{
    ImGuiID mainDockID = dockspaceID;
    ImGuiID rightDockID = ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Right, 0.5f, nullptr, &mainDockID);

    ImGui::DockBuilderDockWindow(CalculatePanelID(0, currentDockspaceID).c_str(), mainDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(1, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(2, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(3, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderFinish(dockspaceID);
}

std::shared_ptr<BehaviorTreeEntityBlueprint> BehaviorTreeEntityBlueprintDocument::GetBehaviorTreeEntityBlueprint() const
{
    return behaviorTreeEntityBlueprint;
}

void BehaviorTreeEntityBlueprintDocument::RenderMenuBar()
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

    UI::ResourceExportPopup(showResourceExportPopup, behaviorTreeEntityBlueprint);

    ImGui::EndMenuBar();
}
