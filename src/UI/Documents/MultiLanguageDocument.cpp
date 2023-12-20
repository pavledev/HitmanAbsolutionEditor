#include <IconsMaterialDesignIcons.h>

#include <UI/Documents/MultiLanguageDocument.h>
#include <UI/Panels/MultiLanguagePanel.h>
#include <UI/Panels/ResourceInfoPanel.h>
#include <UI/Panels/HexViewerPanel.h>
#include <Editor.h>

MultiLanguageDocument::MultiLanguageDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID) : Document(name, icon, type, runtimeResourceID, hasToolBar, dockID)
{
    multiLanguage = std::make_shared<MultiLanguage>();

    std::shared_ptr<MultiLanguagePanel> multiLanguagePanel = std::make_shared<MultiLanguagePanel>("Multi Language", ICON_MDI_TRANSLATE, multiLanguage);
    std::shared_ptr<ResourceInfoPanel> resourceInfoPanel = std::make_shared<ResourceInfoPanel>("Resource Info", ICON_MDI_INFORMATION, multiLanguage);
    std::shared_ptr<HexViewerPanel> headerLibraryHexViewerPanel = std::make_shared<HexViewerPanel>("Header Library Hex Viewer", ICON_MDI_MEMORY, false, multiLanguage);
    std::shared_ptr<HexViewerPanel> resourceLibraryhexViewerPanel = std::make_shared<HexViewerPanel>("Resource Library Hex Viewer", ICON_MDI_MEMORY, true, multiLanguage);

    AddPanel(multiLanguagePanel);
    AddPanel(resourceInfoPanel);
    AddPanel(headerLibraryHexViewerPanel);
    AddPanel(resourceLibraryhexViewerPanel);

    std::weak_ptr<MultiLanguagePanel> multiLanguagePanel2 = multiLanguagePanel;
    std::weak_ptr<ResourceInfoPanel> resourceInfoPanel2 = resourceInfoPanel;

    multiLanguage->SetResourceLoadedCallback([multiLanguagePanel2, resourceInfoPanel2]()
    {
        std::shared_ptr<MultiLanguagePanel> multiLanguagePanel3 = multiLanguagePanel2.lock();
        std::shared_ptr<ResourceInfoPanel> resourceInfoPanel3 = resourceInfoPanel2.lock();

        if (multiLanguagePanel3)
        {
            multiLanguagePanel3->OnResourceLoaded();
        }

        if (resourceInfoPanel3)
        {
            resourceInfoPanel3->OnResourceLoaded();
        }
    });
}

void MultiLanguageDocument::CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize)
{
    ImGuiID mainDockID = dockspaceID;
    ImGuiID rightDockID = ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Right, 0.5f, nullptr, &mainDockID);

    ImGui::DockBuilderDockWindow(CalculatePanelID(0, currentDockspaceID).c_str(), mainDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(1, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(2, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(3, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderFinish(dockspaceID);
}

std::shared_ptr<MultiLanguage> MultiLanguageDocument::GetMultiLanguage() const
{
    return multiLanguage;
}

void MultiLanguageDocument::RenderMenuBar()
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
