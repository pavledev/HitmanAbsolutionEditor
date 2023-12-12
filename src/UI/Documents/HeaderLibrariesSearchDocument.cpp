#include <IconsMaterialDesignIcons.h>

#include <UI/Documents/HeaderLibrariesSearchDocument.h>
#include <UI/Panels/HeaderLibrariesSearchPanel.h>

HeaderLibrariesSearchDocument::HeaderLibrariesSearchDocument(const char* name, const char* icon, const Type type, const ImGuiID dockID) : Document(name, icon, type, -1, true, dockID)
{
    std::shared_ptr<HeaderLibrariesSearchPanel> headerLibrariesSearchPanel = std::make_shared<HeaderLibrariesSearchPanel>("Search Header Libraries", ICON_MDI_FILE_DOCUMENT);

    AddPanel(headerLibrariesSearchPanel);
}

void HeaderLibrariesSearchDocument::CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize)
{
    ImGuiID mainDockID = dockspaceID;

    ImGui::DockBuilderDockWindow(CalculatePanelID(0, currentDockspaceID).c_str(), mainDockID);
    ImGui::DockBuilderFinish(dockspaceID);
}
