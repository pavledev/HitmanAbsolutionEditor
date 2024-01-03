#include <IconsMaterialDesignIcons.h>

#include <UI/Documents/SoundBlendContainerExternalParametersTypeDocument.h>
#include <UI/Panels/SoundBlendContainerExternalParametersTypePanel.h>
#include <UI/Panels/ResourceInfoPanel.h>
#include <UI/Panels/HexViewerPanel.h>
#include <Editor.h>
#include <Utility/ResourceUtility.h>

SoundBlendContainerExternalParametersTypeDocument::SoundBlendContainerExternalParametersTypeDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID) : Document(name, icon, type, runtimeResourceID, hasToolBar, dockID)
{
    soundBlendContainerExternalParametersType = std::make_shared<SoundBlendContainerExternalParametersType>();

    std::shared_ptr<SoundBlendContainerExternalParametersTypePanel> soundBlendContainerExternalParametersTypePanel = std::make_shared<SoundBlendContainerExternalParametersTypePanel>("Sound Blend Container External Parameters Type", ICON_MDI_VOLUME_HIGH, soundBlendContainerExternalParametersType);
    std::shared_ptr<ResourceInfoPanel> resourceInfoPanel = std::make_shared<ResourceInfoPanel>("Resource Info", ICON_MDI_INFORMATION, soundBlendContainerExternalParametersType);
    std::shared_ptr<HexViewerPanel> headerLibraryHexViewerPanel = std::make_shared<HexViewerPanel>("Header Library Hex Viewer", ICON_MDI_MEMORY, false, soundBlendContainerExternalParametersType);
    std::shared_ptr<HexViewerPanel> resourceLibraryhexViewerPanel = std::make_shared<HexViewerPanel>("Resource Library Hex Viewer", ICON_MDI_MEMORY, true, soundBlendContainerExternalParametersType);

    AddPanel(soundBlendContainerExternalParametersTypePanel);
    AddPanel(resourceInfoPanel);
    AddPanel(headerLibraryHexViewerPanel);
    AddPanel(resourceLibraryhexViewerPanel);

    std::weak_ptr<SoundBlendContainerExternalParametersTypePanel> soundBlendContainerExternalParametersTypePanel2 = soundBlendContainerExternalParametersTypePanel;
    std::weak_ptr<ResourceInfoPanel> resourceInfoPanel2 = resourceInfoPanel;

    soundBlendContainerExternalParametersType->SetResourceLoadedCallback([soundBlendContainerExternalParametersTypePanel2, resourceInfoPanel2]()
    {
        std::shared_ptr<SoundBlendContainerExternalParametersTypePanel> soundBlendContainerExternalParametersTypePanel3 = soundBlendContainerExternalParametersTypePanel2.lock();
        std::shared_ptr<ResourceInfoPanel> resourceInfoPanel3 = resourceInfoPanel2.lock();

        if (soundBlendContainerExternalParametersTypePanel3)
        {
            soundBlendContainerExternalParametersTypePanel3->OnResourceLoaded();
        }

        if (resourceInfoPanel3)
        {
            resourceInfoPanel3->OnResourceLoaded();
        }
    });
}

void SoundBlendContainerExternalParametersTypeDocument::CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize)
{
    ImGuiID mainDockID = dockspaceID;
    ImGuiID rightDockID = ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Right, 0.5f, nullptr, &mainDockID);

    ImGui::DockBuilderDockWindow(CalculatePanelID(0, currentDockspaceID).c_str(), mainDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(1, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(2, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(3, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderFinish(dockspaceID);
}

std::shared_ptr<SoundBlendContainerExternalParametersType> SoundBlendContainerExternalParametersTypeDocument::GetSoundBlendContainerExternalParametersType() const
{
    return soundBlendContainerExternalParametersType;
}

void SoundBlendContainerExternalParametersTypeDocument::RenderMenuBar()
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

    UI::ResourceExportPopup(showResourceExportPopup, soundBlendContainerExternalParametersType);

    ImGui::EndMenuBar();
}
