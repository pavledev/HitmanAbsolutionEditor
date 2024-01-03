#include <IconsMaterialDesignIcons.h>

#include <UI/Documents/RenderMaterialEffectDocument.h>
#include <UI/Panels/RenderMaterialEffectPanel.h>
#include <UI/Panels/ShadersPanel.h>
#include <UI/Panels/ResourceInfoPanel.h>
#include <UI/Panels/HexViewerPanel.h>
#include <Editor.h>
#include <Utility/ResourceUtility.h>

RenderMaterialEffectDocument::RenderMaterialEffectDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID) : Document(name, icon, type, runtimeResourceID, hasToolBar, dockID)
{
    renderMaterialEffect = std::make_shared<RenderMaterialEffect>();

    std::shared_ptr<RenderMaterialEffectPanel> renderMaterialEffectPanel = std::make_shared<RenderMaterialEffectPanel>("Render Material Effect", ICON_MDI_PALETTE, renderMaterialEffect);
    std::shared_ptr<ShadersPanel> shadersPanel = std::make_shared<ShadersPanel>("Shaders", ICON_MDI_PALETTE, renderMaterialEffect);
    std::shared_ptr<ResourceInfoPanel> resourceInfoPanel = std::make_shared<ResourceInfoPanel>("Resource Info", ICON_MDI_INFORMATION, renderMaterialEffect);
    std::shared_ptr<HexViewerPanel> headerLibraryHexViewerPanel = std::make_shared<HexViewerPanel>("Header Library Hex Viewer", ICON_MDI_MEMORY, false, renderMaterialEffect);
    std::shared_ptr<HexViewerPanel> resourceLibraryhexViewerPanel = std::make_shared<HexViewerPanel>("Resource Library Hex Viewer", ICON_MDI_MEMORY, true, renderMaterialEffect);

    AddPanel(renderMaterialEffectPanel);
    AddPanel(shadersPanel);
    AddPanel(resourceInfoPanel);
    AddPanel(headerLibraryHexViewerPanel);
    AddPanel(resourceLibraryhexViewerPanel);

    std::weak_ptr<RenderMaterialEffectPanel> renderMaterialEffectPanel2 = renderMaterialEffectPanel;
    std::weak_ptr<ResourceInfoPanel> resourceInfoPanel2 = resourceInfoPanel;

    renderMaterialEffect->SetResourceLoadedCallback([renderMaterialEffectPanel2, resourceInfoPanel2, this]()
    {
        std::shared_ptr<RenderMaterialEffectPanel> renderMaterialEffectPanel3 = renderMaterialEffectPanel2.lock();
        std::shared_ptr<ResourceInfoPanel> resourceInfoPanel3 = resourceInfoPanel2.lock();

        if (renderMaterialEffectPanel3)
        {
            renderMaterialEffectPanel3->OnResourceLoaded();
        }

        if (resourceInfoPanel3)
        {
            resourceInfoPanel3->OnResourceLoaded();
        }
    });
}

void RenderMaterialEffectDocument::CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize)
{
    ImGuiID mainDockID = dockspaceID;
    ImGuiID rightDockID = ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Right, 0.5f, nullptr, &mainDockID);

    ImGui::DockBuilderDockWindow(CalculatePanelID(0, currentDockspaceID).c_str(), mainDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(1, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(2, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(3, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(4, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderFinish(dockspaceID);
}

std::shared_ptr<RenderMaterialEffect> RenderMaterialEffectDocument::GetRenderMaterialEffect() const
{
    return renderMaterialEffect;
}

void RenderMaterialEffectDocument::RenderMenuBar()
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

    UI::ResourceExportPopup(showResourceExportPopup, renderMaterialEffect);

    ImGui::EndMenuBar();
}
