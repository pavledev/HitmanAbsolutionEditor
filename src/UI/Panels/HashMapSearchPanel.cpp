#include <fstream>

#include <IconsMaterialDesignIcons.h>

#include "UI/Panels/HashMapSearchPanel.h"
#include "Utility/StringUtility.h"
#include "Registry/ResourceIDRegistry.h"
#include "Editor.h"

HashMapSearchPanel::HashMapSearchPanel(const char* name, const char* icon) : BasePanel(name, icon)
{
}

void HashMapSearchPanel::Render()
{
    Begin();

    if (!ResourceIDRegistry::GetInstance().IsLoaded())
    {
        End();

        return;
    }

    if (resourceIDs.empty())
    {
        AddResourceIDs();
    }

    ImGui::PushFont(Editor::GetInstance().GetImGuiRenderer()->GetMiddleFont());
    ImGui::Text("Hash Map");
    ImGui::Spacing();

    static ImVec2 childSize = ImVec2(0, 150);
    static std::string hint = std::format("{} Search hash/assembly path...", ICON_MDI_MAGNIFY);
    std::unordered_map<const char*, const char*> filteredHashMap;

    const float textBaseHeight = ImGui::GetTextLineHeightWithSpacing();
    static ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_ScrollY;
    ImVec2 outerSize = ImVec2(0.0f, textBaseHeight * 8);

    FilterHashMap(hint.c_str(), filteredHashMap);

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputTextWithHint("##SearchResource", hint.c_str(), searchText, IM_ARRAYSIZE(searchText));
    ImGui::PopItemWidth();
    ImGui::Spacing();

    if (ImGui::BeginTable("ReferencesTable", 2, tableFlags, outerSize))
    {
        ImGui::TableSetupColumn("Resource ID", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Runtime Resource ID", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;

        clipper.Begin(filteredHashMap.size());

        auto iterator = filteredHashMap.begin();

        while (clipper.Step())
        {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row)
            {
                std::advance(iterator, row);

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text(iterator->first);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text(iterator->second);
            }
        }

        ImGui::EndTable();
    }

    ImGui::PopFont();

    End();
}

void HashMapSearchPanel::AddResourceIDs()
{
    const std::unordered_map<unsigned long long, std::string>& resourceIDs = ResourceIDRegistry::GetInstance().GetResourceIDs();

    for (auto it = resourceIDs.begin(); it != resourceIDs.end(); ++it)
    {
        std::string runtimeResourceID = StringUtility::ConvertValueToHexString(it->first);

        this->resourceIDs.insert(std::make_pair(runtimeResourceID, it->second.c_str()));
    }
}

void HashMapSearchPanel::FilterHashMap(const char* hint, std::unordered_map<const char*, const char*>& filteredHeaderLibraryResourceIDs)
{
    for (auto it = resourceIDs.begin(); it != resourceIDs.end(); ++it)
    {
        if (strcmp(searchText, hint) != 0 && !it->first.contains(searchText) && !strstr(it->second, searchText))
        {
            continue;
        }

        filteredHeaderLibraryResourceIDs.insert(std::make_pair(it->first.c_str(), it->second));
    }
}
