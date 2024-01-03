#include <IconsMaterialDesignIcons.h>

#include <UI/Documents/TemplateEntityDocument.h>
#include <UI/Panels/SceneHierarchyPanel.h>
#include <UI/Panels/TemplateEntityPropertiesPanel.h>
#include <Utility/ResourceUtility.h>

TemplateEntityDocument::TemplateEntityDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID) : Document(name, icon, type, runtimeResourceID, hasToolBar, dockID)
{
    templateEntity = std::make_shared<TemplateEntity>();

    std::shared_ptr<SceneHierarchyPanel> sceneHierarchyPanel = std::make_shared<SceneHierarchyPanel>("Scene Hierarchy", ICON_MDI_VIEW_LIST, templateEntity);
    std::shared_ptr<TemplateEntityPropertiesPanel> templateEntityPropertiesPanel = std::make_shared<TemplateEntityPropertiesPanel>("Entity Properties", ICON_MDI_WRENCH, templateEntity);

    AddPanel(sceneHierarchyPanel);
    AddPanel(templateEntityPropertiesPanel);

    std::weak_ptr<SceneHierarchyPanel> sceneHierarchyPanel2 = sceneHierarchyPanel;
    std::weak_ptr<TemplateEntityPropertiesPanel> templateEntityPropertiesPanel2 = templateEntityPropertiesPanel;

    sceneHierarchyPanel->SetSelectedEntityCallback([templateEntityPropertiesPanel2](std::shared_ptr<SceneHierarchyPanel::EntityTreeNode> selectedEntityTreeNode)
    {
        std::shared_ptr<TemplateEntityPropertiesPanel> templateEntityPropertiesPanel3 = templateEntityPropertiesPanel2.lock();

        if (templateEntityPropertiesPanel3)
        {
            templateEntityPropertiesPanel3->OnSelectedEntityTreeNode(selectedEntityTreeNode);
        }
    });

    templateEntityPropertiesPanel->SetGoToEntityCallback([sceneHierarchyPanel2](const std::shared_ptr<Resource> templateResource, const unsigned int entityIndex)
    {
        std::shared_ptr<SceneHierarchyPanel> sceneHierarchyPanel3 = sceneHierarchyPanel2.lock();

        if (sceneHierarchyPanel3)
        {
            sceneHierarchyPanel3->OnGoToEntity(templateResource, entityIndex);
        }
    });
}

void TemplateEntityDocument::CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize)
{
    ImGuiID mainDockID = dockspaceID;
    ImGuiID leftDockID = ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Left, 0.4f, nullptr, &mainDockID);
    ImGuiID leftDownDockID = ImGui::DockBuilderSplitNode(leftDockID, ImGuiDir_Down, 0.4f, nullptr, &leftDockID);
    ImGuiID rightDockID = ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Right, 0.4f, nullptr, &mainDockID);

    ImGui::DockBuilderDockWindow(CalculatePanelID(0, currentDockspaceID).c_str(), leftDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(1, currentDockspaceID).c_str(), rightDockID);
    /*ImGui::DockBuilderDockWindow(CalculatePanelID(1, currentDockspaceID).c_str(), leftDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(2, currentDockspaceID).c_str(), leftDownDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(3, currentDockspaceID).c_str(), mainDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(4, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(5, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(6, currentDockspaceID).c_str(), rightDockID);*/
    ImGui::DockBuilderFinish(dockspaceID);
}

std::shared_ptr<TemplateEntity> TemplateEntityDocument::GetTemplateEntity() const
{
    return templateEntity;
}

void TemplateEntityDocument::RenderMenuBar()
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

    UI::ResourceExportPopup(showResourceExportPopup, templateEntity);

    ImGui::EndMenuBar();
}
