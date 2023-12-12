#include <IconsMaterialDesignIcons.h>

#include <UI/Documents/HashMapSearchDocument.h>
#include <UI/Panels/HashMapSearchPanel.h>

HashMapSearchDocument::HashMapSearchDocument(const char* name, const char* icon, const Type type, const ImGuiID dockID) : Document(name, icon, type, -1, true, dockID)
{
    std::shared_ptr<HashMapSearchPanel> hashMapSearchPanel = std::make_shared<HashMapSearchPanel>("Search Hash Map", ICON_MDI_FILE_DOCUMENT);

    AddPanel(hashMapSearchPanel);
}

void HashMapSearchDocument::CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize)
{
    ImGuiID mainDockID = dockspaceID;

    ImGui::DockBuilderDockWindow(CalculatePanelID(0, currentDockspaceID).c_str(), mainDockID);
    ImGui::DockBuilderFinish(dockspaceID);
}
