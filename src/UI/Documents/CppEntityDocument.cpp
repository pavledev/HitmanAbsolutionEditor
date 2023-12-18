#include <IconsMaterialDesignIcons.h>

#include <UI/Documents/CppEntityDocument.h>
#include <UI/Panels/CppEntityPropertiesPanel.h>

CppEntityDocument::CppEntityDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID) : Document(name, icon, type, runtimeResourceID, hasToolBar, dockID)
{
    cppEntity = std::make_shared<CppEntity>();

    std::shared_ptr<CppEntityPropertiesPanel> cppEntityPropertiesPanel = std::make_shared<CppEntityPropertiesPanel>("Entity Properties", ICON_MDI_WRENCH, cppEntity);

    AddPanel(cppEntityPropertiesPanel);

    std::weak_ptr<CppEntityPropertiesPanel> cppEntityPropertiesPanel2 = cppEntityPropertiesPanel;

    cppEntity->SetResourceLoadedCallback([cppEntityPropertiesPanel2]()
    {
        std::shared_ptr<CppEntityPropertiesPanel> cppEntityPropertiesPanel3 = cppEntityPropertiesPanel2.lock();

        if (cppEntityPropertiesPanel3)
        {
            cppEntityPropertiesPanel3->OnResourceLoaded();
        }
    });
}

void CppEntityDocument::CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize)
{
    ImGuiID mainDockID = dockspaceID;
    ImGuiID leftDockID = ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Left, 0.4f, nullptr, &mainDockID);
    ImGuiID leftDownDockID = ImGui::DockBuilderSplitNode(leftDockID, ImGuiDir_Down, 0.4f, nullptr, &leftDockID);
    ImGuiID rightDockID = ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Right, 0.4f, nullptr, &mainDockID);

    ImGui::DockBuilderDockWindow(CalculatePanelID(0, currentDockspaceID).c_str(), leftDockID);
    //ImGui::DockBuilderDockWindow(CalculatePanelID(1, currentDockspaceID).c_str(), rightDockID);
    /*ImGui::DockBuilderDockWindow(CalculatePanelID(1, currentDockspaceID).c_str(), leftDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(2, currentDockspaceID).c_str(), leftDownDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(3, currentDockspaceID).c_str(), mainDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(4, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(5, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(6, currentDockspaceID).c_str(), rightDockID);*/
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
