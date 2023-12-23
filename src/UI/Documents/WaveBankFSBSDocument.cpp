#include <IconsMaterialDesignIcons.h>

#include <UI/Documents/WaveBankFSBSDocument.h>
#include <UI/Panels/AudioPlayerPanel.h>
#include <UI/Panels/ResourceInfoPanel.h>
#include <UI/Panels/HexViewerPanel.h>
#include <Editor.h>

WaveBankFSBSDocument::WaveBankFSBSDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID) : Document(name, icon, type, runtimeResourceID, hasToolBar, dockID)
{
    waveBankFSBS = std::make_shared<WaveBankFSBS>();

    std::shared_ptr<AudioPlayerPanel> audioPlayerPanel = std::make_shared<AudioPlayerPanel>("Audio Player", ICON_MDI_VOLUME_HIGH, waveBankFSBS);
    std::shared_ptr<ResourceInfoPanel> resourceInfoPanel = std::make_shared<ResourceInfoPanel>("Resource Info", ICON_MDI_INFORMATION, waveBankFSBS);
    std::shared_ptr<HexViewerPanel> headerLibraryHexViewerPanel = std::make_shared<HexViewerPanel>("Header Library Hex Viewer", ICON_MDI_MEMORY, false, waveBankFSBS);
    std::shared_ptr<HexViewerPanel> resourceLibraryhexViewerPanel = std::make_shared<HexViewerPanel>("Resource Library Hex Viewer", ICON_MDI_MEMORY, true, waveBankFSBS);

    AddPanel(audioPlayerPanel);
    AddPanel(resourceInfoPanel);
    AddPanel(headerLibraryHexViewerPanel);
    AddPanel(resourceLibraryhexViewerPanel);

    std::weak_ptr<AudioPlayerPanel> audioPlayerPanel2 = audioPlayerPanel;
    std::weak_ptr<ResourceInfoPanel> resourceInfoPanel2 = resourceInfoPanel;

    waveBankFSBS->SetResourceLoadedCallback([audioPlayerPanel2, resourceInfoPanel2]()
    {
        std::shared_ptr<AudioPlayerPanel> audioPlayerPanel3 = audioPlayerPanel2.lock();
        std::shared_ptr<ResourceInfoPanel> resourceInfoPanel3 = resourceInfoPanel2.lock();

        if (audioPlayerPanel3)
        {
            audioPlayerPanel3->OnResourceLoaded();
        }

        if (resourceInfoPanel3)
        {
            resourceInfoPanel3->OnResourceLoaded();
        }
    });
}

void WaveBankFSBSDocument::CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize)
{
    ImGuiID mainDockID = dockspaceID;
    ImGuiID rightDockID = ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Right, 0.5f, nullptr, &mainDockID);

    ImGui::DockBuilderDockWindow(CalculatePanelID(0, currentDockspaceID).c_str(), mainDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(1, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(2, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(3, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderFinish(dockspaceID);
}

std::shared_ptr<WaveBankFSBS> WaveBankFSBSDocument::GetWaveBankFSBS() const
{
    return waveBankFSBS;
}

void WaveBankFSBSDocument::RenderMenuBar()
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
