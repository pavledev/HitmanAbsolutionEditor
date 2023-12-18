#pragma once

#include <vector>
#include <memory>

#include "imgui.h"
#include "imgui_internal.h"

#include "UI/Panels/BasePanel.h"

#define EDITOR_CONFIG_SAME_LOCATION_SHARE_LAYOUT              0
#define EDITOR_CONFIG_ONMERGE_OVERWRITE_WITH_SOURCE_LAYOUT    0 // Keep source (dragged) layout, vs keep target layout. Later seems to make more sense.

class Document
{
public:
    enum class Type
    {
        Scene,
        LibraryInfo,
        SearchHeaderLibraries,
        SearchHashMap,
        CppEntity,
        TemplateEntity,
        Texture,
        RenderPrimitive
    };

    Document(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID = -1, const bool open = true, const ImGuiID dockID = 0);
    virtual ~Document() = default;
    const char* GetName() const;
    const Type GetType() const;
    const ImGuiID GetID() const;
    const ImGuiID GetDockID() const;
    bool* GetOpen();
    const ImGuiID GetCurrentDockID() const;
    const ImGuiID GetCurrentLocationID() const;
    const ImGuiID GetPreviousLocationID() const;
    const ImGuiID GetCurrentDockspaceID() const;
    const ImGuiID GetPreviousDockspaceID() const;
    const ImGuiWindowClass* GetToolWindowsClass() const;
    ImGuiWindowClass* GetToolWindowsClass();
    const std::vector<std::shared_ptr<BasePanel>>& GetPanels() const;
    void SetOpen(const bool open);
    void SetDockID(const ImGuiID dockID);
    void SetCurrentDockID(const ImGuiID currentDockID);
    void SetCurrentLocationID(const ImGuiID currentLocationID);
    void SetPreviousLocationID(const ImGuiID previousLocationID);
    void SetCurrentDockspaceID(const ImGuiID currentDockspaceID);
    void SetPreviousDockspaceID(const ImGuiID previousDockspaceID);
    void ResetToolsVisibility();
    void CopyToolsVisibilityFrom(const Document& other);
    ImGuiID CalculateDockspaceID() const;
    void RenderPanels(const bool isLastFocusedDocument);
    void LayoutCopy(ImGuiID srcDockspaceID, ImGuiID dstDockspaceID);
    std::string CalculatePanelID(const int index, const ImGuiID dockspaceID) const;
    void AddPanel(std::shared_ptr<BasePanel> panel);
    virtual void CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize)
    {
    }

    virtual void RenderMenuBar()
    {

    }

protected:
    std::string name;
    const char* icon;
    Type type;
    ImGuiID id;
    ImGuiID dockID;
    bool open;

    ImGuiID currentDockID;
    ImGuiID currentLocationID; // Current Dock node we are docked into _OR_ window ID if floating window
    ImGuiID previousLocationID; // Previous dock node we are docked into _OR_ window ID if floating window
    ImGuiID currentDockspaceID; // Dockspace ID ~~ Hash of LocationID + DocType (with EDITOR_CONFIG_SAME_LOCATION_SHARE_LAYOUT=1)
    ImGuiID previousDockspaceID;
    ImGuiWindowClass toolWindowsClass; // All our tools windows will share the same WindowClass (based on ID) to avoid mixing tools from different top-level editor

    std::vector<std::shared_ptr<BasePanel>> panels;
    std::vector<bool> panelStates;
};
