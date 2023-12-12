#include <fstream>
#include <thread>

#include <IconsMaterialDesignIcons.h>

#include "Glacier/Resource/SResourceHeaderHeader.h"

#include "UI/Panels/HeaderLibrariesSearchPanel.h"
#include "Logger.h"
#include "Utility/StringUtility.h"
#include "Utility/ResourceUtility.h"
#include "IO/BinaryReader.h"
#include "Registry/ResourceIDRegistry.h"
#include "Settings.h"
#include "Editor.h"

HeaderLibrariesSearchPanel::HeaderLibrariesSearchPanel(const char* name, const char* icon) : BasePanel(name, icon)
{
    selectedNodeIndex = -1;

    std::thread thread(&HeaderLibrariesSearchPanel::LoadHeaderLibraryResourceIDs, this);

    thread.detach();

    LoadResourceTypes();
}

void HeaderLibrariesSearchPanel::Render()
{
    if (!Begin())
    {
        return;
    }

    ImGui::PushFont(Editor::GetInstance().GetImGuiRenderer()->GetMiddleFont());
    ImGui::Text("Header Libraries");
    ImGui::Spacing();

    static ImVec2 childSize = ImVec2(0, 150);
    static std::string hint = std::format("{} Search header library...", ICON_MDI_MAGNIFY);
    static std::string hint2 = std::format("{} Search hash/assembly path...", ICON_MDI_MAGNIFY);
    std::map<std::string, bool*> filteredHeaderLibraryResourceIDs;

    FilterHeaderLibraryResourceIDs(hint.c_str(), filteredHeaderLibraryResourceIDs);

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputTextWithHint("##SearchHeaderLibrary", hint.c_str(), headerLibraryResourceID, IM_ARRAYSIZE(headerLibraryResourceID));
    ImGui::PopItemWidth();
    ImGui::Spacing();

    ImGui::BeginChild("Header Libraries", childSize, true);

    for (auto it = filteredHeaderLibraryResourceIDs.begin(); it != filteredHeaderLibraryResourceIDs.end(); ++it)
    {
        ImGui::Checkbox(it->first.c_str(), it->second);
    }

    ImGui::EndChild();
    ImGui::Spacing();

    static bool selectAllHeaderLibraries = true;

    if (ImGui::Checkbox("Select All Header Libraries", &selectAllHeaderLibraries))
    {
        for (auto it = filteredHeaderLibraryResourceIDs.begin(); it != filteredHeaderLibraryResourceIDs.end(); ++it)
        {
            *it->second = selectAllHeaderLibraries;
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Text("Resource Types");
    ImGui::Spacing();
    ImGui::BeginChild("Resource Types", childSize, true);

    for (auto it = resourceTypes.begin(); it != resourceTypes.end(); ++it)
    {
        ImGui::Checkbox(it->first.c_str(), &it->second);
    }

    ImGui::EndChild();
    ImGui::Spacing();

    static bool selectAllResourceTypes = true;

    if (ImGui::Checkbox("Select All Resource Types", &selectAllResourceTypes))
    {
        for (auto it = resourceTypes.begin(); it != resourceTypes.end(); ++it)
        {
            it->second = selectAllResourceTypes;
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Text("Search");

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputTextWithHint("##SearchResource", hint2.c_str(), searchText, IM_ARRAYSIZE(searchText));
    ImGui::PopItemWidth();
    ImGui::Spacing();

    if (ImGui::Button("Search Resource"))
    {
        SearchHeaderLibraries(filteredHeaderLibraryResourceIDs);
    }

    ImGui::Spacing();
    ImGui::BeginChild("Resource Tree", childSize, true);

    for (size_t i = 0; i < treeNodes.size(); ++i)
    {
        RenderTree(treeNodes[i]);
    }

    ImGui::EndChild();

    if (selectedNodeIndex != -1)
    {
        // Update selection state
        // (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
        if (ImGui::GetIO().KeyCtrl)
        {
            selectedNodeIndices.insert(selectedNodeIndex);
            selectedNodeIndex = -1;
        }
        else
        {
            selectedNodeIndices.clear();
            selectedNodeIndices.insert(selectedNodeIndex);
        }
    }

    ImGui::PopFont();

    End();
}

void HeaderLibrariesSearchPanel::RenderTree(TreeNode& treeNode)
{
    ImGui::PushID(&treeNode);

    std::string label;
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_SpanFullWidth;
    const bool isNodeSelected = selectedNodeIndices.contains(treeNode.index);

    if (isNodeSelected)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    if (treeNode.children.size() > 0)
    {
        label = std::format("{} {}", ICON_MDI_FOLDER, treeNode.name);
        nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;

        bool isNodeOpen = ImGui::TreeNodeEx(label.c_str(), nodeFlags);

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            selectedNodeIndex = treeNode.index;
        }

        if (isNodeOpen)
        {
            for (TreeNode& treeNode : treeNode.children)
            {
                RenderTree(treeNode);
            }

            ImGui::TreePop();
        }
    }
    else
    {
        label = std::format("{} {}", ICON_MDI_FILE, treeNode.name);
        nodeFlags |= ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf;

        ImGui::TreeNodeEx(label.c_str(), nodeFlags);

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            selectedNodeIndex = treeNode.index;
        }
    }

    ImGui::PopID();
}

void HeaderLibrariesSearchPanel::LoadHeaderLibraryResourceIDs()
{
    std::ifstream inputFile = std::ifstream("assets/HeaderLibraries.txt");

    if (inputFile.is_open())
    {
        std::string line;

        while (getline(inputFile, line))
        {
            headerLibraryResourceIDs.insert(std::make_pair(line, true));
        }

        inputFile.close();
    }
    else
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to open HeaderLibraries.txt file!");
    }
}

void HeaderLibrariesSearchPanel::LoadResourceTypes()
{
    std::ifstream inputFile = std::ifstream("assets/ResourceTypes.txt");

    if (inputFile.is_open())
    {
        std::string line;

        while (getline(inputFile, line))
        {
            resourceTypes.insert(std::make_pair(line, true));
        }

        inputFile.close();
    }
    else
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to open ResourceTypes.txt file!");
    }
}

void HeaderLibrariesSearchPanel::FilterHeaderLibraryResourceIDs(const char* hint, std::map<std::string, bool*>& filteredHeaderLibraryResourceIDs)
{
    for (auto it = headerLibraryResourceIDs.begin(); it != headerLibraryResourceIDs.end(); ++it)
    {
        if (strcmp(headerLibraryResourceID, hint) != 0 && !it->first.contains(headerLibraryResourceID))
        {
            continue;
        }

        filteredHeaderLibraryResourceIDs.insert(std::make_pair(it->first, &it->second));
    }
}

void HeaderLibrariesSearchPanel::SearchHeaderLibraries(std::map<std::string, bool*>& filteredHeaderLibraryResourceIDs)
{
    treeNodes.clear();
    treeNodes.reserve(filteredHeaderLibraryResourceIDs.size());

    unsigned int nodeIndex = 0;
    TreeNode headerLibraryTreeNode;

    for (auto it = filteredHeaderLibraryResourceIDs.begin(); it != filteredHeaderLibraryResourceIDs.end(); ++it)
    {
        if (!*it->second)
        {
            continue;
        }

        std::string filePath = ResourceUtility::ConvertResourceIDToFilePath(it->first);

        if (!std::filesystem::exists(filePath))
        {
            std::string fileName = filePath.substr(filePath.find_last_of("/") + 1);
            std::string dlcFolderPath = std::format("{}\\DLC", Settings::GetInstance().GetRuntimeFolderPath());

            for (auto& directoryEntry : std::filesystem::recursive_directory_iterator(dlcFolderPath))
            {
                if (directoryEntry.path().extension() != ".pc_headerlib")
                {
                    continue;
                }

                if (directoryEntry.path().filename() == fileName)
                {
                    filePath = directoryEntry.path().string();

                    break;
                }
            }
        }

        BinaryReader binaryReader = BinaryReader(filePath);
        SResourceHeaderHeader resourceHeaderHeader = binaryReader.Read<SResourceHeaderHeader>();

        binaryReader.Skip(resourceHeaderHeader.m_nReferencesChunkSize);
        binaryReader.Skip(0x30); //Size of BIN2 header + size of SHeaderLibrary

        unsigned int headerLibraryChunkCount = binaryReader.Read<unsigned int>();

        for (unsigned int i = 0; i < headerLibraryChunkCount; ++i)
        {
            const unsigned int chunkOffset = binaryReader.GetPosition();

            binaryReader.Seek(chunkOffset + 0x2C);

            unsigned int resourceHeadersStartOffset = binaryReader.GetPosition();
            resourceHeadersStartOffset += binaryReader.Read<unsigned int>();

            binaryReader.Seek(resourceHeadersStartOffset - 4);

            unsigned int resourceCount = binaryReader.Read<unsigned int>();
            std::vector<std::string> resourceTypes;

            resourceTypes.reserve(resourceCount);

            for (unsigned int j = 0; j < resourceCount; ++j)
            {
                binaryReader.Seek(resourceHeadersStartOffset + j * 0xC, SeekOrigin::Begin);

                unsigned int resourceHeaderOffset = binaryReader.GetPosition();
                resourceHeaderOffset += binaryReader.Read<unsigned int>();

                binaryReader.Seek(resourceHeaderOffset);

                std::string resourceType = ResourceUtility::ConvertResourceTypeToString(binaryReader.Read<unsigned int>());

                resourceTypes.push_back(resourceType);
            }

            binaryReader.Seek(chunkOffset + 0x44);

            unsigned int ridMappingIDsStartOffset = static_cast<unsigned int>(binaryReader.GetPosition());
            ridMappingIDsStartOffset += binaryReader.Read<unsigned int>();

            binaryReader.Seek(ridMappingIDsStartOffset - 4);

            unsigned int ridMappingIDsCount = binaryReader.Read<unsigned int>();

            for (unsigned int j = 0; j < ridMappingIDsCount; ++j)
            {
                const std::string& resourceType = resourceTypes[j];

                if (!this->resourceTypes[resourceType])
                {
                    continue;
                }

                unsigned long long runtimeResourceID = binaryReader.Read<unsigned long long>();
                std::string runtimeResourceID2 = StringUtility::ConvertValueToHexString(runtimeResourceID);
                std::string resourceID = ResourceIDRegistry::GetInstance().GetResourceID(runtimeResourceID);

                if (runtimeResourceID2.contains(searchText) || resourceID.contains(searchText))
                {
                    TreeNode resourceTreeNode;

                    resourceTreeNode.index = nodeIndex++;
                    resourceTreeNode.name = std::format("{} {}", runtimeResourceID2, resourceID);

                    headerLibraryTreeNode.children.push_back(resourceTreeNode);
                }
            }

            binaryReader.Seek(chunkOffset, SeekOrigin::Begin);
            binaryReader.Seek(0x50, SeekOrigin::Current);
        }

        if (headerLibraryTreeNode.children.size() > 0)
        {
            headerLibraryTreeNode.index = nodeIndex++;
            headerLibraryTreeNode.name = it->first;

            treeNodes.push_back(headerLibraryTreeNode);
        }
    }
}
