#include <IconsMaterialDesignIcons.h>

#include <UI/Documents/WaveBankFSBSDocument.h>
#include <UI/Panels/AudioPlayerPanel.h>
#include <UI/Panels/ResourceInfoPanel.h>
#include <UI/Panels/HexViewerPanel.h>
#include <Utility/FileDialog.h>
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
    static std::string patchLabel = std::format("{} Patch Back To Game", ICON_MDI_CONTENT_SAVE);
    static bool showResourceExportPopup = false;

    if (ImGui::BeginMenu("Export"))
    {
        if (ImGui::MenuItem(exportResourceLabel.c_str()))
        {
            showResourceExportPopup = true;
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Import"))
    {
        if (ImGui::MenuItem(patchLabel.c_str()))
        {
            std::string filePath = FileDialog::OpenFile("OGG Files (*.ogg)\0*.ogg\0All Files (*.*)\0*.*\0");

            if (!filePath.empty())
            {
                std::weak_ptr<AudioPlayerPanel> audioPlayerPanel2 = std::static_pointer_cast<AudioPlayerPanel>(panels[0]);
                std::shared_ptr<AudioPlayerPanel> audioPlayerPanel3 = audioPlayerPanel2.lock();
                unsigned int indexToReplace = 0;
                
                if (audioPlayerPanel3)
                {
                    indexToReplace = audioPlayerPanel3->GetSelectedAudioSampleIndex();
                }

                if (waveBankFSBS->PatchResourceLibrary(filePath, indexToReplace))
                {
                    if (audioPlayerPanel3)
                    {
                        audioPlayerPanel3->Refresh();
                    }

                    ImGui::OpenPopup("Patch Success");
                }
                else
                {
                    ImGui::OpenPopup("Patch Failed");
                }
            }
        }

        ImGui::EndMenu();
    }

    UI::ResourceExportPopup(showResourceExportPopup, waveBankFSBS);

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();

    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Patch Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("FSBS patched successfully!");
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Patch Failed", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Failed to patch. Check the log for details.");
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::EndMenuBar();
}
