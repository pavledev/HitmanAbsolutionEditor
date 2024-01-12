#include <IconsMaterialDesignIcons.h>

#include <UI/Documents/RenderPrimitiveDocument.h>
#include <UI/Panels/SceneHierarchyPanel2.h>
#include <UI/Panels/ModelViewerPanel.h>
#include <UI/Panels/ComponentPropertiesPanel.h>
#include <UI/Panels/ResourceInfoPanel.h>
#include <UI/Panels/HexViewerPanel.h>
#include <Editor.h>
#include <Utility/UI.h>

RenderPrimitiveDocument::RenderPrimitiveDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID) : Document(name, icon, type, runtimeResourceID, hasToolBar, dockID)
{
    renderPrimitive = std::make_shared<RenderPrimitive>();

    std::shared_ptr<SceneHierarchyPanel2> sceneHierarchyPanel = std::make_shared<SceneHierarchyPanel2>("Scene Hierarchy", ICON_MDI_VIEW_LIST, renderPrimitive);
    std::shared_ptr<ComponentPropertiesPanel> componentPropertiesPanel = std::make_shared<ComponentPropertiesPanel>("Properties", ICON_MDI_WRENCH, renderPrimitive);
    std::shared_ptr<ModelViewerPanel> modelViewerPanel = std::make_shared<ModelViewerPanel>("Model Viewer", ICON_MDI_MONITOR);
    std::shared_ptr<ResourceInfoPanel> resourceInfoPanel = std::make_shared<ResourceInfoPanel>("Resource Info", ICON_MDI_INFORMATION, renderPrimitive);
    std::shared_ptr<HexViewerPanel> headerLibraryHexViewerPanel = std::make_shared<HexViewerPanel>("Header Library Hex Viewer", ICON_MDI_MEMORY, false, renderPrimitive);
    std::shared_ptr<HexViewerPanel> resourceLibraryhexViewerPanel = std::make_shared<HexViewerPanel>("Resource Library Hex Viewer", ICON_MDI_MEMORY, true, renderPrimitive);

    AddPanel(sceneHierarchyPanel);
    AddPanel(componentPropertiesPanel);
    AddPanel(modelViewerPanel);
    AddPanel(resourceInfoPanel);
    AddPanel(headerLibraryHexViewerPanel);
    AddPanel(resourceLibraryhexViewerPanel);

    sceneHierarchyPanel->SetRenderer3D(modelViewerPanel->GetRenderer3D());
    componentPropertiesPanel->SetRenderer3D(modelViewerPanel->GetRenderer3D());

    std::weak_ptr<SceneHierarchyPanel2> sceneHierarchyPanel2 = sceneHierarchyPanel;
    std::weak_ptr<ModelViewerPanel> modelViewerPanel2 = modelViewerPanel;
    std::weak_ptr<ResourceInfoPanel> resourceInfoPanel2 = resourceInfoPanel;

    renderPrimitive->SetResourceLoadedCallback([sceneHierarchyPanel2, modelViewerPanel2, resourceInfoPanel2, this]()
    {
        std::shared_ptr<SceneHierarchyPanel2> sceneHierarchyPanel3 = sceneHierarchyPanel2.lock();
        std::shared_ptr<ModelViewerPanel> modelViewerPanel3 = modelViewerPanel2.lock();
        std::shared_ptr<ResourceInfoPanel> resourceInfoPanel3 = resourceInfoPanel2.lock();

        OnResourceLoaded();

        if (modelViewerPanel3)
        {
            modelViewerPanel3->OnResourceLoaded();
        }

        if (sceneHierarchyPanel3)
        {
            sceneHierarchyPanel3->OnResourceLoaded();
        }

        if (resourceInfoPanel3)
        {
            resourceInfoPanel3->OnResourceLoaded();
        }
    });
}

void RenderPrimitiveDocument::CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize)
{
    ImGuiID mainDockID = dockspaceID;
    ImGuiID leftDockID = ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Left, 0.2f, nullptr, &mainDockID);
    ImGuiID leftDownDockID = ImGui::DockBuilderSplitNode(leftDockID, ImGuiDir_Down, 0.4f, nullptr, &leftDockID);
    ImGuiID rightDockID = ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Right, 0.2f, nullptr, &mainDockID);

    ImGui::DockBuilderDockWindow(CalculatePanelID(0, currentDockspaceID).c_str(), leftDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(1, currentDockspaceID).c_str(), leftDownDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(2, currentDockspaceID).c_str(), mainDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(3, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(4, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(5, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderFinish(dockspaceID);
}

std::shared_ptr<RenderPrimitive> RenderPrimitiveDocument::GetRenderPrimitive() const
{
    return renderPrimitive;
}

void RenderPrimitiveDocument::RenderMenuBar()
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

    UI::ResourceExportPopup(showResourceExportPopup, renderPrimitive);

    ImGui::EndMenuBar();
}

void RenderPrimitiveDocument::OnResourceLoaded()
{
    renderPrimitive->Deserialize();
}
