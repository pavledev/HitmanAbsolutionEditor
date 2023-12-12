#include <IconsMaterialDesignIcons.h>

#include <UI/Documents/LibraryInfoDocument.h>
#include <UI/Panels/LibraryPanel.h>

LibraryInfoDocument::LibraryInfoDocument(const char* name, const char* icon, const Type type, const ImGuiID dockID) : Document(name, icon, type, -1, true, dockID)
{
    std::shared_ptr<LibraryPanel> libraryPanel = std::make_shared<LibraryPanel>("Library Info", ICON_MDI_FILE_DOCUMENT);

    AddPanel(libraryPanel);
}

void LibraryInfoDocument::CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize)
{
    ImGuiID mainDockID = dockspaceID;

    ImGui::DockBuilderDockWindow(CalculatePanelID(0, currentDockspaceID).c_str(), mainDockID);
    ImGui::DockBuilderFinish(dockspaceID);
}
