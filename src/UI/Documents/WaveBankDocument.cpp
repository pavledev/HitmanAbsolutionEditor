#include <IconsMaterialDesignIcons.h>

#include <UI/Documents/WaveBankDocument.h>
#include <UI/Panels/WaveBankPanel.h>
#include <UI/Panels/ResourceInfoPanel.h>
#include <UI/Panels/HexViewerPanel.h>
#include <Editor.h>

WaveBankDocument::WaveBankDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID) : Document(name, icon, type, runtimeResourceID, hasToolBar, dockID)
{
    waveBank = std::make_shared<WaveBank>();

    std::shared_ptr<WaveBankPanel> waveBankPanel = std::make_shared<WaveBankPanel>("Wave Bank", ICON_MDI_VOLUME_HIGH, waveBank);
    std::shared_ptr<ResourceInfoPanel> resourceInfoPanel = std::make_shared<ResourceInfoPanel>("Resource Info", ICON_MDI_INFORMATION, waveBank);
    std::shared_ptr<HexViewerPanel> headerLibraryHexViewerPanel = std::make_shared<HexViewerPanel>("Header Library Hex Viewer", ICON_MDI_MEMORY, false, waveBank);
    std::shared_ptr<HexViewerPanel> resourceLibraryhexViewerPanel = std::make_shared<HexViewerPanel>("Resource Library Hex Viewer", ICON_MDI_MEMORY, true, waveBank);

    AddPanel(waveBankPanel);
    AddPanel(resourceInfoPanel);
    AddPanel(headerLibraryHexViewerPanel);
    AddPanel(resourceLibraryhexViewerPanel);

    std::weak_ptr<WaveBankPanel> waveBankPanel2 = waveBankPanel;
    std::weak_ptr<ResourceInfoPanel> resourceInfoPanel2 = resourceInfoPanel;

    waveBank->SetResourceLoadedCallback([waveBankPanel2, resourceInfoPanel2]()
    {
        std::shared_ptr<WaveBankPanel> waveBankPanel3 = waveBankPanel2.lock();
        std::shared_ptr<ResourceInfoPanel> resourceInfoPanel3 = resourceInfoPanel2.lock();

        if (waveBankPanel3)
        {
            waveBankPanel3->OnResourceLoaded();
        }

        if (resourceInfoPanel3)
        {
            resourceInfoPanel3->OnResourceLoaded();
        }
    });
}

void WaveBankDocument::CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize)
{
    ImGuiID mainDockID = dockspaceID;
    ImGuiID rightDockID = ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Right, 0.5f, nullptr, &mainDockID);

    ImGui::DockBuilderDockWindow(CalculatePanelID(0, currentDockspaceID).c_str(), mainDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(1, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(2, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(3, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderFinish(dockspaceID);
}

std::shared_ptr<WaveBank> WaveBankDocument::GetWaveBank() const
{
    return waveBank;
}

void WaveBankDocument::RenderMenuBar()
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

    UI::ResourceExportPopup(showResourceExportPopup, waveBank);

    ImGui::EndMenuBar();
}
