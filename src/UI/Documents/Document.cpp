#include <sstream>
#include <iomanip>
#include <format>

#include "UI/Documents/Document.h"
#include "Utility/StringUtility.h"

Document::Document(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool open, ImGuiID dockID) : icon(icon), type(type), open(open), dockID(dockID)
{
    this->name = std::format(" {} {}\t\t", icon, name);

    if (runtimeResourceID != -1)
    {
        id = ImHashStr(std::format("{}_{}", name, runtimeResourceID).c_str(), 0);
    }
    else
    {
        id = ImHashStr(name, 0);
    }

    wasOpened = open;
    currentDockID = 0;
    currentLocationID = 0;
    previousLocationID = 0;
    currentDockspaceID = 0;
    previousDockspaceID = 0;

    ResetToolsVisibility();
}

const char* Document::GetName() const
{
    return name.c_str();
}

const Document::Type Document::GetType() const
{
    return type;
}

const ImGuiID Document::GetID() const
{
    return id;
}

const ImGuiID Document::GetDockID() const
{
    return dockID;
}

bool* Document::GetOpen()
{
    return &open;
}

const bool Document::GetWasOpened() const
{
    return wasOpened;
}

const ImGuiID Document::GetCurrentDockID() const
{
    return currentDockID;
}

const ImGuiID Document::GetCurrentLocationID() const
{
    return currentLocationID;
}

const ImGuiID Document::GetPreviousLocationID() const
{
    return previousLocationID;
}

const ImGuiID Document::GetCurrentDockspaceID() const
{
    return currentDockspaceID;
}

const ImGuiID Document::GetPreviousDockspaceID() const
{
    return previousDockspaceID;
}

const ImGuiWindowClass* Document::GetToolWindowsClass() const
{
    return &toolWindowsClass;
}

ImGuiWindowClass* Document::GetToolWindowsClass()
{
    return &toolWindowsClass;
}

const std::vector<std::shared_ptr<BasePanel>>& Document::GetPanels() const
{
    return panels;
}

void Document::SetOpen(const bool open)
{
    this->open = open;
    wasOpened = true;
}

void Document::SetDockID(const ImGuiID dockID)
{
    this->dockID = dockID;
}

void Document::SetCurrentDockID(const ImGuiID currentDockID)
{
    this->currentDockID = currentDockID;
}

void Document::SetCurrentLocationID(const ImGuiID currentLocationID)
{
    this->currentLocationID = currentLocationID;
}

void Document::SetPreviousLocationID(const ImGuiID previousLocationID)
{
    this->previousLocationID = previousLocationID;
}

void Document::SetCurrentDockspaceID(const ImGuiID currentDockspaceID)
{
    this->currentDockspaceID = currentDockspaceID;
}

void Document::SetPreviousDockspaceID(const ImGuiID previousDockspaceID)
{
    this->previousDockspaceID = previousDockspaceID;
}

void Document::ResetToolsVisibility()
{
    for (size_t i = 0; i < panelStates.size(); ++i)
    {
        panelStates[i] = true;
    }
}

void Document::CopyToolsVisibilityFrom(const Document& other)
{
    panelStates = other.panelStates;
}

ImGuiID Document::CalculateDockspaceID() const
{
    ImU32 id = 0;

#if EDITOR_CONFIG_SAME_LOCATION_SHARE_LAYOUT
    // Shared dockspace per-document type in a same tab-bar
    // - Pros: Always sync by default.
    // - Cons: Ambiguous/weird merging/re-docking.
    id = ImHashData(&type, sizeof(type), currentLocationID);
#else
    // Unique dockspace per document
    // - Cons: Not sync by default. Create more windows/data/garbage.
    //id = ImHashStr("Dockspace", 0, this->id);
    id = ImHashData(&this->id, sizeof(this->id), currentLocationID);
#endif
    return id;
}

void Document::RenderPanels(const bool isLastFocusedDocument)
{
    /*if (!isLastFocusedDocument)
    {
        return;
    }*/

    for (size_t i = 0; i < panels.size(); ++i)
    {
        bool isPanelVisible = panelStates[i];

        if (!isPanelVisible)
        {
            continue;
        }

        std::string id = CalculatePanelID(i, currentDockspaceID);

        panels[i]->SetID(id);

        if (!isLastFocusedDocument)
        {
            if (ImGuiWindow* window = ImGui::FindWindowByName(id.c_str()))
            {
                if (window->DockNode == NULL || ImGui::DockNodeGetRootNode(window->DockNode)->ID != currentDockspaceID)
                {
                    continue;
                }
            }
        }

        ImGui::SetNextWindowClass(&toolWindowsClass);

        panels[i]->Render();
    }
}

void Document::LayoutCopy(ImGuiID srcDockspaceID, ImGuiID dstDockspaceID)
{
    IM_ASSERT(srcDockspaceID != 0);
    IM_ASSERT(dstDockspaceID != 0);

    // Helper to build an array of strings pointer into the same contiguous memory buffer.
    struct ContiguousStringArrayBuilder
    {
        ImVector<char>  buffer;
        ImVector<int>   offsets;

        void AddEntry(const char* data, size_t data_len)
        {
            offsets.push_back(buffer.Size);
            int offset = buffer.Size;
            buffer.resize(buffer.Size + (int)data_len);
            memcpy(buffer.Data + offset, data, data_len);
        }

        void BuildPointerArray(ImVector<const char*>* out_array)
        {
            out_array->resize(offsets.Size);
            for (int n = 0; n < offsets.Size; n++)
                (*out_array)[n] = buffer.Data + offsets[n];
        }
    };

    // Build an array of remapped names
    ContiguousStringArrayBuilder namePairsBuilder;
    // Iterate all known tools
    namePairsBuilder.buffer.resize(50 * panels.size());

    for (size_t i = 0; i < panels.size(); i++)
    {
        std::string panelID = CalculatePanelID(i, srcDockspaceID);

        namePairsBuilder.AddEntry(panelID.c_str(), panelID.length() + 1);

        std::string panelID2 = CalculatePanelID(i, dstDockspaceID);

        namePairsBuilder.AddEntry(panelID2.c_str(), panelID2.length() + 1);
    }

    // Build the same array with char* pointers at it is the input of DockBuilderCopyDockspace() (may change its signature?)
    ImVector<const char*> windowRemapPairs;

    namePairsBuilder.BuildPointerArray(&windowRemapPairs);

    // Perform the cloning
    ImGui::DockBuilderCopyDockSpace(srcDockspaceID, dstDockspaceID, &windowRemapPairs);
    ImGui::DockBuilderFinish(dstDockspaceID);
}

std::string Document::CalculatePanelID(const int index, const ImGuiID dockspaceID) const
{
    std::stringstream stream;

    stream << "##" << std::hex << std::setw(8) << std::setfill('0') << dockspaceID;

    return panels[index]->GetName() + stream.str();
}

void Document::AddPanel(std::shared_ptr<BasePanel> panel)
{
    panels.push_back(panel);
    panelStates.push_back(true);
}
