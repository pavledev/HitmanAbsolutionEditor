#include <IconsMaterialDesignIcons.h>

#include <UI/Documents/TextureDocument.h>
#include <UI/Panels/ResourceViewerPanel.h>
#include <UI/Panels/ResourceInfoPanel.h>
#include <UI/Panels/HexViewerPanel.h>

TextureDocument::TextureDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID) : Document(name, icon, type, runtimeResourceID, hasToolBar, dockID)
{
    texture = std::make_shared<Texture>();

    std::shared_ptr<ResourceViewerPanel> resourceViewerPanel = std::make_shared<ResourceViewerPanel>("Resource Viewer", ICON_MDI_IMAGE, texture);
    std::shared_ptr<ResourceInfoPanel> resourceInfoPanel = std::make_shared<ResourceInfoPanel>("Resource Info", ICON_MDI_INFORMATION, texture);
    std::shared_ptr<HexViewerPanel> headerLibraryHexViewerPanel = std::make_shared<HexViewerPanel>("Header Library Hex Viewer", ICON_MDI_MEMORY, false, texture);
    std::shared_ptr<HexViewerPanel> resourceLibraryhexViewerPanel = std::make_shared<HexViewerPanel>("Resource Library Hex Viewer", ICON_MDI_MEMORY, true, texture);

    AddPanel(resourceViewerPanel);
    AddPanel(resourceInfoPanel);
    AddPanel(headerLibraryHexViewerPanel);
    AddPanel(resourceLibraryhexViewerPanel);

    std::weak_ptr<ResourceInfoPanel> resourceInfoPanel2 = resourceInfoPanel;

    texture->SetResourceLoadedCallback([resourceInfoPanel2]()
    {
        std::shared_ptr<ResourceInfoPanel> resourceInfoPanel3 = resourceInfoPanel2.lock();

        if (resourceInfoPanel3)
        {
            resourceInfoPanel3->OnResourceLoaded();
        }
    });
}

void TextureDocument::CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize)
{
    ImGuiID mainDockID = dockspaceID;
    ImGuiID rightDockID = ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Right, 0.5f, nullptr, &mainDockID);

    ImGui::DockBuilderDockWindow(CalculatePanelID(0, currentDockspaceID).c_str(), mainDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(1, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(2, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(3, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderFinish(dockspaceID);
}

std::shared_ptr<Texture> TextureDocument::GetTexture() const
{
    return texture;
}

void TextureDocument::RenderMenuBar()
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
