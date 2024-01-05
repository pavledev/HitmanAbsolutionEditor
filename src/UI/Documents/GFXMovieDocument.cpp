#include <IconsMaterialDesignIcons.h>

#include <UI/Documents/GFXMovieDocument.h>
#include <UI/Panels/TextureViewerPanel.h>
#include <UI/Panels/GFXMoviePanel.h>
#include <UI/Panels/ResourceInfoPanel.h>
#include <UI/Panels/HexViewerPanel.h>
#include <Editor.h>

GFXMovieDocument::GFXMovieDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID) : Document(name, icon, type, runtimeResourceID, hasToolBar, dockID)
{
    gfxMovie = std::make_shared<GFXMovie>();
    sliderValue = 0.f;

    std::shared_ptr<TextureViewerPanel> textureViewerPanel = std::make_shared<TextureViewerPanel>("Texture Viewer", ICON_MDI_IMAGE, nullptr, &sliderValue);
    std::shared_ptr<GFXMoviePanel> gfxMoviePanel = std::make_shared<GFXMoviePanel>("GFX Movie", ICON_MDI_IMAGE, gfxMovie);
    std::shared_ptr<ResourceInfoPanel> resourceInfoPanel = std::make_shared<ResourceInfoPanel>("Resource Info", ICON_MDI_INFORMATION, gfxMovie);
    std::shared_ptr<HexViewerPanel> headerLibraryHexViewerPanel = std::make_shared<HexViewerPanel>("Header Library Hex Viewer", ICON_MDI_MEMORY, false, gfxMovie);
    std::shared_ptr<HexViewerPanel> resourceLibraryhexViewerPanel = std::make_shared<HexViewerPanel>("Resource Library Hex Viewer", ICON_MDI_MEMORY, true, gfxMovie);

    AddPanel(textureViewerPanel);
    AddPanel(gfxMoviePanel);
    AddPanel(resourceInfoPanel);
    AddPanel(headerLibraryHexViewerPanel);
    AddPanel(resourceLibraryhexViewerPanel);

    std::weak_ptr<TextureViewerPanel> textureViewerPanel2 = textureViewerPanel;
    std::weak_ptr<GFXMoviePanel> gfxMoviePanel2 = gfxMoviePanel;
    std::weak_ptr<ResourceInfoPanel> resourceInfoPanel2 = resourceInfoPanel;

    gfxMovie->SetResourceLoadedCallback([textureViewerPanel2, gfxMoviePanel2, resourceInfoPanel2]()
    {
        std::shared_ptr<TextureViewerPanel> textureViewerPanel3 = textureViewerPanel2.lock();
        std::shared_ptr<GFXMoviePanel> gfxMoviePanel3 = gfxMoviePanel2.lock();
        std::shared_ptr<ResourceInfoPanel> resourceInfoPanel3 = resourceInfoPanel2.lock();

        if (textureViewerPanel3)
        {
            textureViewerPanel3->OnResourceLoaded();
        }

        if (gfxMoviePanel3)
        {
            gfxMoviePanel3->OnResourceLoaded();
        }

        if (resourceInfoPanel3)
        {
            resourceInfoPanel3->OnResourceLoaded();
        }
    });

    gfxMoviePanel->SetSelectedDDSTextureCallback([textureViewerPanel2](ID3D11ShaderResourceView* textureView, const unsigned int textureWidth, const unsigned int textureHeight)
    {
        std::shared_ptr<TextureViewerPanel> textureViewerPanel3 = textureViewerPanel2.lock();

        if (textureViewerPanel3)
        {
            textureViewerPanel3->OnSelectedDDSTexture(textureView, textureWidth, textureHeight);
        }
    });
}

void GFXMovieDocument::CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize)
{
    ImGuiID mainDockID = dockspaceID;
    ImGuiID rightDockID = ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Right, 0.5f, nullptr, &mainDockID);
    ImGuiID downDockID = ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Down, 0.3f, nullptr, &mainDockID);

    ImGui::DockBuilderDockWindow(CalculatePanelID(0, currentDockspaceID).c_str(), mainDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(1, currentDockspaceID).c_str(), downDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(2, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(3, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(4, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderFinish(dockspaceID);
}

std::shared_ptr<GFXMovie> GFXMovieDocument::GetGFXMovie() const
{
    return gfxMovie;
}

void GFXMovieDocument::RenderMenuBar()
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

    UI::ResourceExportPopup(showResourceExportPopup, gfxMovie);

    ImGui::EndMenuBar();
}

void GFXMovieDocument::RenderToolBar()
{
    ImGui::PushFont(Editor::GetInstance().GetImGuiRenderer()->GetMiddleFont());

    ImVec2 screenPosition = ImGui::GetCursorScreenPos();
    const float toolbarHeight = ImGui::GetFrameHeightWithSpacing() + ImGui::GetStyle().FramePadding.y * 2;

    if (UI::BeginToolbar("TextureDocumentToolBar", screenPosition, ImVec2(0, toolbarHeight)))
    {
        std::shared_ptr<TextureViewerPanel> textureViewerPanel = std::static_pointer_cast<TextureViewerPanel>(panels[0]);
        const ImVec2 framePadding = ImGui::GetStyle().FramePadding;
        screenPosition = ImGui::GetCursorPos();

        screenPosition.x += framePadding.x;
        screenPosition.y += framePadding.y;

        ImGui::SetCursorPos(screenPosition);

        RenderRGBAChannelButtons();

        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine();

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Zoom");

        ImGui::SameLine();

        ImGui::PushItemWidth(300);

        if (ImGui::SliderFloat("##ZoomSlider", &sliderValue, 0.f, 1.f, ""))
        {
            textureViewerPanel->OnZoomSliderValueChange();
        }

        ImGui::PopItemWidth();

        ImGui::SameLine();

        RenderZoomPercentageText();

        ImGui::SameLine();

        if (UI::IconButton("  " ICON_MDI_FIT_TO_SCREEN, " Scale To Fit"))
        {
            textureViewerPanel->SetZoomMode(TextureViewerPanel::ZoomMode::Fit);
        }

        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + framePadding.x);

        if (UI::IconButton("  " ICON_MDI_FIT_TO_SCREEN, " Scale To Fill"))
        {
            textureViewerPanel->SetZoomMode(TextureViewerPanel::ZoomMode::Fill);
        }
    }

    UI::EndToolbar();

    ImGui::PopFont();
}

void GFXMovieDocument::RenderZoomPercentageText() const
{
    std::shared_ptr<TextureViewerPanel> textureViewerPanel = std::static_pointer_cast<TextureViewerPanel>(panels[0]);
    double displayedZoomLevel = textureViewerPanel->CalculateDisplayedZoomLevel();
    const std::string zoomLevelPercent = std::format("{}%%", static_cast<int>(displayedZoomLevel * static_cast<double>(100)));
    const TextureViewerPanel::ZoomMode zoomMode = textureViewerPanel->GetZoomMode();

    switch (zoomMode)
    {
        case TextureViewerPanel::ZoomMode::Custom:
        {
            ImGui::Text(zoomLevelPercent.c_str());

            break;
        }
        case TextureViewerPanel::ZoomMode::Fit:
        {
            const std::string zoomModeWithPercentFormat = std::format("Fit ({})", zoomLevelPercent);

            ImGui::Text(zoomModeWithPercentFormat.c_str());

            break;
        }
        case TextureViewerPanel::ZoomMode::Fill:
        {
            const std::string zoomModeWithPercentFormat = std::format("Fill ({})", zoomLevelPercent);

            ImGui::Text(zoomModeWithPercentFormat.c_str());

            break;
        }
    }
}

void GFXMovieDocument::RenderRGBAChannelButtons() const
{
    std::shared_ptr<TextureViewerPanel> textureViewerPanel = std::static_pointer_cast<TextureViewerPanel>(panels[0]);
    static bool isRedChannelEnabled = true;
    static bool isGreenChannelEnabled = true;
    static bool isBlueChannelEnabled = true;
    static bool isAlphaChannelEnabled = true;
    static bool isRedChannelButtonPressed = false;
    static bool isGreenChannelButtonPressed = false;
    static bool isBlueChannelButtonPressed = false;
    static bool isAlphaChannelButtonPressed = false;

    if (isRedChannelEnabled)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    }

    if (ImGui::Button("R", ImVec2(30, 0)))
    {
        isRedChannelButtonPressed = true;
    }

    if (isRedChannelEnabled)
    {
        ImGui::PopStyleColor(2);
    }

    if (isRedChannelButtonPressed)
    {
        isRedChannelEnabled = !isRedChannelEnabled;
        isRedChannelButtonPressed = false;

        textureViewerPanel->SetIsRedChannelEnabled(isRedChannelEnabled);
    }

    ImGui::SameLine();

    if (isGreenChannelEnabled)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
    }

    if (ImGui::Button("G", ImVec2(30, 0)))
    {
        isGreenChannelButtonPressed = true;
    }

    if (isGreenChannelEnabled)
    {
        ImGui::PopStyleColor(2);
    }

    if (isGreenChannelButtonPressed)
    {
        isGreenChannelEnabled = !isGreenChannelEnabled;
        isGreenChannelButtonPressed = false;

        textureViewerPanel->SetIsGreenChannelEnabled(isGreenChannelEnabled);
    }

    ImGui::SameLine();

    if (isBlueChannelEnabled)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
    }

    if (ImGui::Button("B", ImVec2(30, 0)))
    {
        isBlueChannelButtonPressed = true;
    }

    if (isBlueChannelEnabled)
    {
        ImGui::PopStyleColor(2);
    }

    if (isBlueChannelButtonPressed)
    {
        isBlueChannelEnabled = !isBlueChannelEnabled;
        isBlueChannelButtonPressed = false;

        textureViewerPanel->SetIsBlueChannelEnabled(isBlueChannelEnabled);
    }

    ImGui::SameLine();

    if (isAlphaChannelEnabled)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    //const bool hasAlpha = DirectX::HasAlpha(texture->GetDXGIFormat());
    const bool hasAlpha = true;

    ImGui::BeginDisabled(!hasAlpha);

    if (ImGui::Button("A", ImVec2(30, 0)))
    {
        isAlphaChannelButtonPressed = true;
    }

    ImGui::EndDisabled();

    if (isAlphaChannelEnabled)
    {
        ImGui::PopStyleColor(2);
    }

    if (isAlphaChannelButtonPressed)
    {
        isAlphaChannelEnabled = !isAlphaChannelEnabled;
        isAlphaChannelButtonPressed = false;

        textureViewerPanel->SetIsAlphaChannelEnabled(isAlphaChannelEnabled);
    }
}
