#include <format>
#include <fstream>
#include <thread>
#include <filesystem>
#include <algorithm>

#include <IconsMaterialDesignIcons.h>

#include "UI/Panels/ResourceBrowserPanel.h"
#include "Utility/StringUtility.h"
#include "Logger.h"
#include "Utility/ResourceUtility.h"
#include "Utility/UI.h"
#include "Utility/FileDialog.h"
#include "UI/Panels/HexViewerPanel.h"
#include "UI/Panels/ResourceInfoPanel.h"
#include "UI/Panels/SceneHierarchyPanel2.h"
#include "UI/Panels/ComponentPropertiesPanel.h"
#include "UI/Panels/BoneHierarchyPanel.h"
#include "Registry/ResourceInfoRegistry.h"
#include "UI/Documents/TemplateEntityDocument.h"
#include "UI/Documents/TextureDocument.h"
#include "UI/Documents/CppEntityDocument.h"
#include "UI/Documents/CppEntityBlueprintDocument.h"
#include "UI/Documents/TextListDocument.h"
#include "UI/Documents/LocalizationDocument.h"
#include "UI/Documents/MultiLanguageDocument.h"
#include "UI/Documents/RenderMaterialEntityTypeDocument.h"
#include "UI/Documents/RenderMaterialInstanceDocument.h"
#include "UI/Documents/RenderMaterialEffectDocument.h"
#include "UI/Documents/SoundBlendContainerExternalParametersTypeDocument.h"
#include "UI/Documents/SoundBlendContainerExternalParametersBlueprintDocument.h"
#include "UI/Documents/WaveBankFSBFDocument.h"
#include "UI/Documents/WaveBankFSBMDocument.h"
#include "UI/Documents/WaveBankFSBSDocument.h"
#include "UI/Documents/WaveBankDocument.h"
#include "UI/Documents/WaveBankFXDocument.h"
#include "UI/Documents/FlashMovieDocument.h"
#include "UI/Documents/GFXMovieDocument.h"
#include "UI/Documents/AnimationDatabaseDocument.h"
#include "UI/Documents/SoundDefinitionsDocument.h"
#include "UI/Documents/GlobalResourceIndexDocument.h"
#include "UI/Documents/BehaviorTreeEntityBlueprintDocument.h"
#include "UI/Documents/CompositionBlueprintDocument.h"
#include "UI/Documents/PackageListDocument.h"
#include "UI/Documents/ScatterDataDocument.h"
#include "UI/Documents/FabricResourceEntityDocument.h"
#include "UI/Documents/RenderPrimitiveDocument.h"
#include "UI/Documents/ClothDocument.h"
#include "UI/Documents/BoneRigDocument.h"
#include "UI/Documents/PhysicsDocument.h"
#include "Resources/WaveBankFSBS.h"
#include "Editor.h"

ResourceBrowserPanel::ResourceBrowserPanel(const char* name, const char* icon) : BasePanel(name, icon)
{
    isInputTextActive = false;
    selectedNodeIndex = -1;
    showResourceExportPopup = false;
    showImportJsonPopup = false;
    showPatchPopup = false;
    showFsbsPatchPopup = false;
    showBatchTeliExportPopup = false;
    showBatchTeliImportPopup = false;
    batchExportFormatIndex = 0;

    LoadResourceTypes();
    AddRootResourceNodes();
}

ResourceBrowserPanel::~ResourceBrowserPanel()
{
}

void ResourceBrowserPanel::Render()
{
    if (!Begin())
    {
        return;
    }

    ImGui::PushFont(Editor::GetInstance().GetImGuiRenderer()->GetMiddleFont());

    const ImVec2 framePadding = ImGui::GetStyle().FramePadding;
    float itemWidth = ImGui::GetContentRegionAvail().x - (UI::GetIconButtonSize("  " ICON_MDI_FILTER, "").x + 2.0f * framePadding.x);

    ImGui::PushItemWidth(itemWidth);

    if (ImGui::InputTextWithHint("##SearchResource", "Search Resource...", resourceName, IM_ARRAYSIZE(resourceName)))
    {
        assemblyNode.children.clear();
        modulesNode.children.clear();

        AddRootResourceNodes();
    }

    ImGui::PopItemWidth();

    isInputTextActive = ImGui::IsItemActive();

    if (isInputTextActive &&
        (!(assemblyNode.children.size() == 1 && assemblyNode.children[0].name.empty()) ||
            !(modulesNode.children.size() == 1 && modulesNode.children[0].name.empty())))
    {
        assemblyNode.children.clear();
        modulesNode.children.clear();

        AddRootResourceNodes();
    }

    ImGui::SameLine();

    if (UI::IconButton("  " ICON_MDI_FILTER, ""))
    {
        ImGui::OpenPopup("Filter Resources");
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();

    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Filter Resources", nullptr))
    {
        ImGui::Text("Resource Types");
        ImGui::Spacing();

        ImGui::BeginChild("ScrollingRegion", ImVec2(300, 300));

        for (auto it = resourceTypes.begin(); it != resourceTypes.end(); ++it)
        {
            ImGui::Checkbox(it->first.c_str(), &it->second);
        }

        ImGui::EndChild();
        ImGui::Spacing();
        ImGui::Spacing();

        static bool selectAll = true;

        if (ImGui::Checkbox("Select All", &selectAll))
        {
            for (auto it = resourceTypes.begin(); it != resourceTypes.end(); ++it)
            {
                it->second = selectAll;
            }
        }

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        if (UI::IconButton("  " ICON_MDI_CHECK, " Ok "))
        {
            ImGui::CloseCurrentPopup();

            assemblyNode.children.clear();
            modulesNode.children.clear();

            AddRootResourceNodes();
        }

        ImGui::EndPopup();
    }

    ImGui::Spacing();

    if (ResourceInfoRegistry::GetInstance().IsResourcesInfoLoaded())
    {
        RenderTree(assemblyNode, assemblyNode.name);
        RenderTree(modulesNode, modulesNode.name);

        UI::ResourceExportPopup(showResourceExportPopup, resource);

        // Batch TELI export popup
        if (showBatchTeliExportPopup)
        {
            ImGui::OpenPopup("Batch Export TELI Files");
        }

        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImVec2 modalSize = ImVec2(650, 500);
            ImVec2 centerPosition = ImVec2(
                viewport->GetCenter().x - modalSize.x / 2,
                viewport->GetCenter().y - modalSize.y / 2
            );

            ImGui::SetNextWindowSize(modalSize);
            ImGui::SetNextWindowPos(centerPosition);

            if (ImGui::BeginPopupModal("Batch Export TELI Files", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::PushFont(Editor::GetInstance().GetImGuiRenderer()->GetMiddleFont());

                // Language selection
                ImGui::TextUnformatted("Select Languages to Export:");
                ImGui::Spacing();

                ImGui::BeginChild("LanguageSelection", ImVec2(0, 150), true);

                for (size_t i = 0; i < detectedLanguages.size(); ++i)
                {
                    bool selected = selectedLanguages[i];
                    std::string label = detectedLanguages[i];

                    if (label == "_en") label = "English";
                    else if (label == "_fr") label = "French";
                    else if (label == "_it") label = "Italian";
                    else if (label == "_de") label = "German";
                    else if (label == "_es") label = "Spanish";
                    else if (label == "_ru") label = "Russian";
                    else if (label == "_pl") label = "Polish";
                    else if (label == "_pt") label = "Portuguese";
                    else if (label == "_ja") label = "Japanese";
                    else if (label == "_tr") label = "Turkish";
                    else if (label == "_cn") label = "Chinese";
                    else if (label == "_ko") label = "Korean";

                    if (ImGui::Checkbox(label.c_str(), &selected))
                    {
                        selectedLanguages[i] = selected;
                    }
                }

                ImGui::EndChild();

                ImGui::Spacing();

                // Select All / Deselect All
                if (ImGui::Button("Select All"))
                {
                    for (size_t i = 0; i < selectedLanguages.size(); ++i)
                        selectedLanguages[i] = true;
                }

                ImGui::SameLine();

                if (ImGui::Button("Deselect All"))
                {
                    for (size_t i = 0; i < selectedLanguages.size(); ++i)
                        selectedLanguages[i] = false;
                }

                ImGui::Spacing();
                ImGui::Spacing();

                // Export format
                ImGui::TextUnformatted("Export Format:");
                ImGui::Spacing();

                ImGui::RadioButton("JSON File (.TEXTLIST.JSON)", &batchExportFormatIndex, 0);
                ImGui::RadioButton("Raw File (.TELI)", &batchExportFormatIndex, 1);

                ImGui::Spacing();
                ImGui::Spacing();

                // Output folder
                static std::string batchOutputFolder;

                ImGui::TextUnformatted("Output Folder:");

                float windowWidth = ImGui::GetContentRegionAvail().x;
                float buttonWidth = UI::GetIconButtonSize(ICON_MDI_FOLDER, "").x;
                float inputTextWidth = windowWidth - buttonWidth - ImGui::GetStyle().ItemSpacing.x;

                ImGui::PushItemWidth(inputTextWidth);
                ImGui::InputText("##BatchOutputFolder", &batchOutputFolder);
                ImGui::PopItemWidth();
                ImGui::SameLine();

                if (ImGui::Button(ICON_MDI_FOLDER))
                {
                    batchOutputFolder = FileDialog::OpenFolder();
                }

                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Spacing();

                // Export button
                if (UI::IconButton("  " ICON_MDI_EXPORT, " Export All "))
                {
                    showBatchTeliExportPopup = false;
                    ImGui::CloseCurrentPopup();

                    if (!batchOutputFolder.empty())
                    {
                        // Collect language filters
                        std::vector<std::string> langFilters;

                        for (size_t i = 0; i < detectedLanguages.size(); ++i)
                        {
                            if (selectedLanguages[i])
                            {
                                langFilters.push_back(detectedLanguages[i]);
                            }
                        }

                        // Collect TELI files with language filter
                        batchTeliHashes.clear();
                        batchTeliNames.clear();
                        CollectTeliChildren(batchTeliParentPath, langFilters, batchTeliHashes, batchTeliNames);

                        std::string formatStr = (batchExportFormatIndex == 0) ? "Json" : "Raw";
                        BatchExportTeliFiles(batchOutputFolder, formatStr);
                    }
                }

                ImGui::SameLine();

                if (UI::IconButton("  " ICON_MDI_CLOSE, " Cancel "))
                {
                    showBatchTeliExportPopup = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::PopFont();
                ImGui::EndPopup();
            }
        }

        // Batch TELI import
        if (showBatchTeliImportPopup)
        {
            showBatchTeliImportPopup = false;

            std::string inputFolder = FileDialog::OpenFolder();

            if (!inputFolder.empty())
            {
                BatchImportTeliFiles(inputFolder);
            }
        }

        if (showImportJsonPopup && resource && resource->IsResourceLoaded())
        {
            showImportJsonPopup = false;

            std::string filePath;
            std::string extension;

            FileDialog::OpenSaveFileDialog("", ".json", "JSON Files (*.json)\0*.json\0All Files (*.*)\0*.*\0", filePath, extension);

            if (!filePath.empty())
            {
                std::shared_ptr<TextList> textList = std::static_pointer_cast<TextList>(resource);
                textList->ImportFromJson(filePath);
            }
        }

        if (showPatchPopup && resource && resource->IsResourceLoaded())
        {
            showPatchPopup = false;

            std::shared_ptr<TextList> textList = std::static_pointer_cast<TextList>(resource);
            textList->PatchResourceLibrary();
        }

        if (showFsbsPatchPopup && resource && resource->IsResourceLoaded())
        {
            showFsbsPatchPopup = false;

            std::string filePath = FileDialog::OpenFile("OGG Files (*.ogg)\0*.ogg\0All Files (*.*)\0*.*\0");

            if (!filePath.empty())
            {
                std::shared_ptr<WaveBankFSBS> waveBankFSBS = std::static_pointer_cast<WaveBankFSBS>(resource);

                if (waveBankFSBS->PatchResourceLibrary(filePath))
                {
                    Logger::GetInstance().Log(Logger::Level::Info, "FSBS patched successfully from Resource Browser");
                }
                else
                {
                    Logger::GetInstance().Log(Logger::Level::Error, "Failed to patch FSBS from Resource Browser");
                }
            }
        }
    }

    ImGui::PopFont();

    End();
}

void ResourceBrowserPanel::RenderTree(ResourceNode& parentNode, std::string parentPath)
{
    ImGui::PushID(&parentNode);

    std::string label;
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_SpanFullWidth;
    const bool isNodeSelected = selectedNodeIndex == parentNode.index;

    if (isNodeSelected)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    if (isInputTextActive)
    {
        ImGui::SetNextItemOpen(false);
    }

    if (parentNode.children.size() > 0)
    {
        label = std::format("{} {}", ICON_MDI_FOLDER, parentNode.name);
        nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;

        bool isNodeOpen = ImGui::TreeNodeEx(label.c_str(), nodeFlags);

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            selectedNodeIndex = parentNode.index;
        }

        if (ImGui::BeginPopupContextItem())
        {
            RenderFolderContextMenu(parentNode, parentPath);
            ImGui::EndPopup();
        }

        if (isNodeOpen)
        {
            if (parentNode.children.size() == 1 && parentNode.children[0].name.empty())
            {
                AddChildren(parentNode, parentPath);
            }

            for (ResourceNode& resourceNode : parentNode.children)
            {
                if (parentPath.ends_with("/"))
                {
                    RenderTree(resourceNode, parentPath + resourceNode.name);
                }
                else
                {
                    RenderTree(resourceNode, parentPath + "/" + resourceNode.name);
                }
            }

            ImGui::TreePop();
        }
    }
    else
    {
        label = std::format("{} {}", ICON_MDI_FILE, parentNode.name);
        nodeFlags |= ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf;

        ImGui::TreeNodeEx(label.c_str(), nodeFlags);

        if ((ImGui::IsItemClicked() || ImGui::IsItemClicked(ImGuiPopupFlags_MouseButtonRight)) && !ImGui::IsItemToggledOpen())
        {
            selectedNodeIndex = parentNode.index;
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
        {
            CreateResourceDocument(parentNode);
        }

        if (ImGui::BeginPopupContextItem())
        {
            RenderContextMenu(parentNode);

            ImGui::EndPopup();
        }
    }

    ImGui::PopID();
}

void ResourceBrowserPanel::RenderContextMenu(ResourceNode& resourceNode)
{
    static std::string exportResourceLabel = std::format("{} Export Resource", ICON_MDI_EXPORT);
    static std::string importJsonLabel = std::format("{} Import JSON (Replace Localization)", ICON_MDI_IMPORT);
    static std::string patchResourceLabel = std::format("{} Patch Resource Library (Write Back)", ICON_MDI_CONTENT_SAVE);

    const ResourceInfoRegistry::ResourceInfo& resourceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(resourceNode.hash);

    if (ImGui::MenuItem(exportResourceLabel.c_str()))
    {
        showResourceExportPopup = true;

        resource = ResourceUtility::CreateResource(resourceInfo.type);
        std::string resourceName = ResourceUtility::GetResourceName(resourceInfo.resourceID);

        resource->SetHash(resourceInfo.hash);
        resource->SetResourceID(resourceInfo.resourceID);
        resource->SetHeaderLibraries(&resourceInfo.headerLibraries);
        resource->SetName(resourceName);

        std::thread thread(&ResourceBrowserPanel::LoadResource, this, resource, resourceNode, true);

        thread.detach();
    }

    if (resourceInfo.type == "TELI")
    {
        if (ImGui::MenuItem(importJsonLabel.c_str()))
        {
            showImportJsonPopup = true;

            resource = ResourceUtility::CreateResource(resourceInfo.type);
            std::string resourceName = ResourceUtility::GetResourceName(resourceInfo.resourceID);

            resource->SetHash(resourceInfo.hash);
            resource->SetResourceID(resourceInfo.resourceID);
            resource->SetHeaderLibraries(&resourceInfo.headerLibraries);
            resource->SetName(resourceName);
            pendingResourceNode = resourceNode;

            std::thread thread(&ResourceBrowserPanel::LoadResource, this, resource, resourceNode, true);

            thread.detach();
        }

        if (ImGui::MenuItem(patchResourceLabel.c_str()))
        {
            showPatchPopup = true;

            resource = ResourceUtility::CreateResource(resourceInfo.type);
            std::string resourceName = ResourceUtility::GetResourceName(resourceInfo.resourceID);

            resource->SetHash(resourceInfo.hash);
            resource->SetResourceID(resourceInfo.resourceID);
            resource->SetHeaderLibraries(&resourceInfo.headerLibraries);
            resource->SetName(resourceName);
            pendingResourceNode = resourceNode;

            std::thread thread(&ResourceBrowserPanel::LoadResource, this, resource, resourceNode, true);

            thread.detach();
        }
    }

    if (resourceInfo.type == "FSBS")
    {
        static std::string fsbsPatchLabel = std::format("{} Patch Back To Game", ICON_MDI_CONTENT_SAVE);

        if (ImGui::MenuItem(fsbsPatchLabel.c_str()))
        {
            showFsbsPatchPopup = true;

            resource = ResourceUtility::CreateResource(resourceInfo.type);
            std::string resourceName = ResourceUtility::GetResourceName(resourceInfo.resourceID);

            resource->SetHash(resourceInfo.hash);
            resource->SetResourceID(resourceInfo.resourceID);
            resource->SetHeaderLibraries(&resourceInfo.headerLibraries);
            resource->SetName(resourceName);
            pendingResourceNode = resourceNode;

            std::thread thread(&ResourceBrowserPanel::LoadResource, this, resource, resourceNode, true);

            thread.detach();
        }
    }
}

void ResourceBrowserPanel::AddChildren(ResourceNode& parentNode, const std::string& parentPath)
{
    parentNode.children.clear();

    std::unordered_map<unsigned long long, ResourceInfoRegistry::ResourceInfo> resourcesInfo = ResourceInfoRegistry::GetInstance().GetResourcesInfo();
    std::map<std::string, ResourceNode> resourceNodes;

    for (auto it = resourcesInfo.begin(); it != resourcesInfo.end(); ++it)
    {
        ResourceInfoRegistry::ResourceInfo& resourceInfo = it->second;

        //Skip assembly paths if parent is module
        if (parentPath.starts_with("m") && resourceInfo.resourceID.starts_with("[a"))
        {
            continue;
        }

        if (!resourceTypes[resourceInfo.type] || !resourceInfo.resourceID.contains(resourceName))
        {
            continue;
        }

        if (resourceInfo.resourceID.starts_with("[" + parentPath) || resourceInfo.resourceID.starts_with("[[" + parentPath)) //[[ is case with MATE resources
        {
            const size_t index = resourceInfo.resourceID.find(parentPath);
            std::string pathPart = resourceInfo.resourceID.substr(index + parentPath.length());

            ResourceNode resourceNode;
            bool isFolder = StringUtility::Count(pathPart, '/') > 0;
            bool isAspectEntityResource = false;

            if (pathPart.starts_with("/"))
            {
                const size_t index2 = pathPart.find('/', 1);

                if (index2 != -1)
                {
                    pathPart = pathPart.substr(1, pathPart.find('/', 1) - 1);
                }
                else
                {
                    pathPart = pathPart.substr(1);
                    isFolder = false;
                }
            }
            else
            {
                if (parentPath.starts_with("m"))
                {
                    if (pathPart.contains('?'))
                    {
                        pathPart = pathPart.substr(0, pathPart.find('?') + 1);
                        isFolder = true;
                    }
                }
                else
                {
                    if (pathPart.starts_with("](["))
                    {
                        std::string references = resourceInfo.resourceID.substr(resourceInfo.resourceID.find("(") + 1, resourceInfo.resourceID.find(")") - resourceInfo.resourceID.find("(") - 1);
                        std::vector<std::string> references2 = StringUtility::Split(references, ',');
                        const size_t referenceCount = references2.size();

                        pathPart = "";

                        for (size_t i = 0; i < referenceCount; ++i)
                        {
                            pathPart += ResourceUtility::GetResourceName(references2[i]);

                            if (i < referenceCount - 1)
                            {
                                pathPart += ", ";
                            }
                        }

                        isFolder = false;
                        isAspectEntityResource = true;
                    }
                    else
                    {
                        if (parentPath.ends_with("/"))
                        {
                            pathPart = pathPart.substr(0, pathPart.find('/'));
                        }
                        else
                        {
                            continue; //Case when pathPart contains part of parent's folder name
                        }
                    }
                }
            }

            if (!isFolder)
            {
                if (!isAspectEntityResource)
                {
                    pathPart = pathPart.substr(0, pathPart.find("]"));
                }

                pathPart += " (" + StringUtility::ToUpperCase(resourceInfo.type) + ")";
            }
            else
            {
                if (pathPart.starts_with("aspectdummy"))
                {
                    pathPart = pathPart.substr(0, pathPart.find("]"));
                }
            }

            if (resourceNodes.contains(pathPart))
            {
                continue;
            }

            if (isFolder)
            {
                resourceNode.children.push_back(ResourceNode());
            }
            else
            {
                resourceNode.hash = resourceInfo.hash;
            }

            resourceNode.name = pathPart;
            resourceNode.index = nodeIndex++;

            resourceNodes[pathPart] = resourceNode;
        }
    }

    parentNode.children.reserve(resourceNodes.size());

    for (auto it = resourceNodes.begin(); it != resourceNodes.end(); ++it)
    {
        parentNode.children.push_back(it->second);
    }
}

void ResourceBrowserPanel::AddRootResourceNodes()
{
    nodeIndex = 0;

    assemblyNode.index = nodeIndex++;
    assemblyNode.hash = -1;
    assemblyNode.name = "assembly:/";
    modulesNode.index = nodeIndex++;
    modulesNode.hash = -1;
    modulesNode.name = "modules:/";

    assemblyNode.children.push_back(ResourceNode());
    modulesNode.children.push_back(ResourceNode());
}

void ResourceBrowserPanel::LoadResourceTypes()
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

void ResourceBrowserPanel::LoadResource(std::shared_ptr<Resource> resource, const ResourceNode& resourceNode, const bool loadBackReferences)
{
    const ResourceInfoRegistry::ResourceInfo& resourceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(resourceNode.hash);

    if (resourceInfo.headerLibraries.size() > 0)
    {
        resource->LoadResource(0, resourceInfo.headerLibraries[0].chunkIndex, resourceInfo.headerLibraries[0].indexInLibrary, true, true, loadBackReferences);
    }
    else
    {
        resource->LoadResource(0, -1, -1, true, true, loadBackReferences);
    }
}

void ResourceBrowserPanel::CreateResourceDocument(const ResourceNode& resourceNode)
{
    const ImGuiID defaultDockID = Editor::GetInstance().GetLastActiveDocument() ? Editor::GetInstance().GetLastActiveDocument()->GetCurrentDockID() : 0;
    const ResourceInfoRegistry::ResourceInfo& resourceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(resourceNode.hash);
    std::string resourceName = ResourceUtility::GetResourceName(resourceInfo.resourceID);

    std::shared_ptr<Resource> resource;
    std::shared_ptr<Document> resourceDocument;

    if (resourceInfo.type == "CPPT")
    {
        std::shared_ptr<CppEntityDocument> templateEntityDocument = std::make_shared<CppEntityDocument>(resourceName.c_str(), ICON_MDI_LANGUAGE_CPP, Document::Type::CppEntity, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(templateEntityDocument->GetCppEntity());
        resourceDocument = std::static_pointer_cast<Document>(templateEntityDocument);
    }
    else if (resourceInfo.type == "CBLU")
    {
        std::shared_ptr<CppEntityBlueprintDocument> cppEntityBlueprintDocument = std::make_shared<CppEntityBlueprintDocument>(resourceName.c_str(), ICON_MDI_LANGUAGE_CPP, Document::Type::CppEntityBlueprint, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(cppEntityBlueprintDocument->GetCppEntityBlueprint());
        resourceDocument = std::static_pointer_cast<Document>(cppEntityBlueprintDocument);
    }
    else if (resourceInfo.type == "TEMP")
    {
        std::shared_ptr<TemplateEntityDocument> templateEntityDocument = std::make_shared<TemplateEntityDocument>(resourceName.c_str(), ICON_MDI_FILE_DOCUMENT, Document::Type::TemplateEntity, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(templateEntityDocument->GetTemplateEntity());
        resourceDocument = std::static_pointer_cast<Document>(templateEntityDocument);
    }
    else if (resourceInfo.type == "TEXT")
    {
        std::shared_ptr<TextureDocument> textureDocument = std::make_shared<TextureDocument>(resourceName.c_str(), ICON_MDI_IMAGE, Document::Type::Texture, resourceInfo.hash, true, defaultDockID);

        resource = std::static_pointer_cast<Resource>(textureDocument->GetTexture());
        resourceDocument = std::static_pointer_cast<Document>(textureDocument);
    }
    else if (resourceInfo.type == "TELI")
    {
        std::shared_ptr<TextListDocument> textureDocument = std::make_shared<TextListDocument>(resourceName.c_str(), ICON_MDI_TEXT_BOX, Document::Type::TextList, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(textureDocument->GetTextList());
        resourceDocument = std::static_pointer_cast<Document>(textureDocument);
    }
    else if (resourceInfo.type == "LOCR")
    {
        std::shared_ptr<LocalizationDocument> localizationDocument = std::make_shared<LocalizationDocument>(resourceName.c_str(), ICON_MDI_TRANSLATE, Document::Type::Localization, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(localizationDocument->GetLocalization());
        resourceDocument = std::static_pointer_cast<Document>(localizationDocument);
    }
    else if (resourceInfo.type == "LOCM")
    {
        std::shared_ptr<MultiLanguageDocument> multiLanguageDocument = std::make_shared<MultiLanguageDocument>(resourceName.c_str(), ICON_MDI_TRANSLATE, Document::Type::MultiLanguage, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(multiLanguageDocument->GetMultiLanguage());
        resourceDocument = std::static_pointer_cast<Document>(multiLanguageDocument);
    }
    else if (resourceInfo.type == "MATT")
    {
        std::shared_ptr<RenderMaterialEntityTypeDocument> renderMaterialEntityTypeDocument = std::make_shared<RenderMaterialEntityTypeDocument>(resourceName.c_str(), ICON_MDI_PALETTE, Document::Type::RenderMaterialEntityType, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(renderMaterialEntityTypeDocument->GetRenderMaterialEntityType());
        resourceDocument = std::static_pointer_cast<Document>(renderMaterialEntityTypeDocument);
    }
    else if (resourceInfo.type == "MATI")
    {
        std::shared_ptr<RenderMaterialInstanceDocument> renderMaterialInstanceDocument = std::make_shared<RenderMaterialInstanceDocument>(resourceName.c_str(), ICON_MDI_PALETTE, Document::Type::RenderMaterialInstance, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(renderMaterialInstanceDocument->GetRenderMaterialInstance());
        resourceDocument = std::static_pointer_cast<Document>(renderMaterialInstanceDocument);
    }
    else if (resourceInfo.type == "MATE")
    {
        std::shared_ptr<RenderMaterialEffectDocument> renderMaterialEffectDocument = std::make_shared<RenderMaterialEffectDocument>(resourceName.c_str(), ICON_MDI_PALETTE, Document::Type::RenderMaterialEffect, resourceInfo.hash, true, defaultDockID);

        resource = std::static_pointer_cast<Resource>(renderMaterialEffectDocument->GetRenderMaterialEffect());
        resourceDocument = std::static_pointer_cast<Document>(renderMaterialEffectDocument);
    }
    else if (resourceInfo.type == "SBPD")
    {
        std::shared_ptr<SoundBlendContainerExternalParametersTypeDocument> soundBlendContainerExternalParametersTypeDocument = std::make_shared<SoundBlendContainerExternalParametersTypeDocument>(resourceName.c_str(), ICON_MDI_VOLUME_HIGH, Document::Type::SoundBlendContainerExternalParametersType, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(soundBlendContainerExternalParametersTypeDocument->GetSoundBlendContainerExternalParametersType());
        resourceDocument = std::static_pointer_cast<Document>(soundBlendContainerExternalParametersTypeDocument);
    }
    else if (resourceInfo.type == "SBPB")
    {
        std::shared_ptr<SoundBlendContainerExternalParametersBlueprintDocument> soundBlendContainerExternalParametersBlueprintDocument = std::make_shared<SoundBlendContainerExternalParametersBlueprintDocument>(resourceName.c_str(), ICON_MDI_VOLUME_HIGH, Document::Type::SoundBlendContainerExternalParametersBlueprint, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(soundBlendContainerExternalParametersBlueprintDocument->GetSoundBlendContainerExternalParametersBlueprint());
        resourceDocument = std::static_pointer_cast<Document>(soundBlendContainerExternalParametersBlueprintDocument);
    }
    else if (resourceInfo.type == "FSBF")
    {
        std::shared_ptr<WaveBankFSBFDocument> waveBankFSBFDocument = std::make_shared<WaveBankFSBFDocument>(resourceName.c_str(), ICON_MDI_VOLUME_HIGH, Document::Type::WaveBankFSBF, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(waveBankFSBFDocument->GetWaveBankFSBF());
        resourceDocument = std::static_pointer_cast<Document>(waveBankFSBFDocument);
    }
    else if (resourceInfo.type == "FSBM")
    {
        std::shared_ptr<WaveBankFSBMDocument> waveBankFSBMDocument = std::make_shared<WaveBankFSBMDocument>(resourceName.c_str(), ICON_MDI_VOLUME_HIGH, Document::Type::WaveBankFSBM, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(waveBankFSBMDocument->GetWaveBankFSBM());
        resourceDocument = std::static_pointer_cast<Document>(waveBankFSBMDocument);
    }
    else if (resourceInfo.type == "FSBS")
    {
        std::shared_ptr<WaveBankFSBSDocument> waveBankFSBSDocument = std::make_shared<WaveBankFSBSDocument>(resourceName.c_str(), ICON_MDI_VOLUME_HIGH, Document::Type::WaveBankFSBS, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(waveBankFSBSDocument->GetWaveBankFSBS());
        resourceDocument = std::static_pointer_cast<Document>(waveBankFSBSDocument);
    }
    else if (resourceInfo.type == "WAVB")
    {
        std::shared_ptr<WaveBankDocument> waveBankDocument = std::make_shared<WaveBankDocument>(resourceName.c_str(), ICON_MDI_VOLUME_HIGH, Document::Type::WaveBank, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(waveBankDocument->GetWaveBank());
        resourceDocument = std::static_pointer_cast<Document>(waveBankDocument);
    }
    else if (resourceInfo.type == "WBFX")
    {
        std::shared_ptr<WaveBankFXDocument> waveBankFXDocument = std::make_shared<WaveBankFXDocument>(resourceName.c_str(), ICON_MDI_VOLUME_HIGH, Document::Type::WaveBankFX, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(waveBankFXDocument->GetWaveBankFX());
        resourceDocument = std::static_pointer_cast<Document>(waveBankFXDocument);
    }
    else if (resourceInfo.type == "SWFF")
    {
        std::shared_ptr<FlashMovieDocument> flashMovieDocument = std::make_shared<FlashMovieDocument>(resourceName.c_str(), ICON_MDI_IMAGE, Document::Type::FlashMovie, resourceInfo.hash, true, defaultDockID);

        resource = std::static_pointer_cast<Resource>(flashMovieDocument->GetFlashMovie());
        resourceDocument = std::static_pointer_cast<Document>(flashMovieDocument);
    }
    else if (resourceInfo.type == "GFXF")
    {
        std::shared_ptr<GFXMovieDocument> gfxMovieDocument = std::make_shared<GFXMovieDocument>(resourceName.c_str(), ICON_MDI_IMAGE, Document::Type::GFXMovie, resourceInfo.hash, true, defaultDockID);

        resource = std::static_pointer_cast<Resource>(gfxMovieDocument->GetGFXMovie());
        resourceDocument = std::static_pointer_cast<Document>(gfxMovieDocument);
    }
    else if (resourceInfo.type == "ChrT")
    {
        std::shared_ptr<AnimationDatabaseDocument> animationDatabaseDocument = std::make_shared<AnimationDatabaseDocument>(resourceName.c_str(), ICON_MDI_DATABASE, Document::Type::AnimationDatabase, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(animationDatabaseDocument->GetAnimationDatabase());
        resourceDocument = std::static_pointer_cast<Document>(animationDatabaseDocument);
    }
    else if (resourceInfo.type == "SDEF")
    {
        std::shared_ptr<SoundDefinitionsDocument> soundDefinitionsDocument = std::make_shared<SoundDefinitionsDocument>(resourceName.c_str(), ICON_MDI_VOLUME_HIGH, Document::Type::SoundDefinitions, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(soundDefinitionsDocument->GetSoundDefinitions());
        resourceDocument = std::static_pointer_cast<Document>(soundDefinitionsDocument);
    }
    else if (resourceInfo.type == "GIDX")
    {
        std::shared_ptr<GlobalResourceIndexDocument> globalResourceIndexDocument = std::make_shared<GlobalResourceIndexDocument>(resourceName.c_str(), ICON_MDI_VIEW_LIST, Document::Type::SoundDefinitions, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(globalResourceIndexDocument->GetGlobalResourceIndex());
        resourceDocument = std::static_pointer_cast<Document>(globalResourceIndexDocument);
    }
    else if (resourceInfo.type == "AIBB")
    {
        std::shared_ptr<BehaviorTreeEntityBlueprintDocument> behaviorTreeEntityBlueprintDocument = std::make_shared<BehaviorTreeEntityBlueprintDocument>(resourceName.c_str(), ICON_MDI_VIEW_LIST, Document::Type::SoundDefinitions, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(behaviorTreeEntityBlueprintDocument->GetBehaviorTreeEntityBlueprint());
        resourceDocument = std::static_pointer_cast<Document>(behaviorTreeEntityBlueprintDocument);
    }
    else if (resourceInfo.type == "MUCB")
    {
        std::shared_ptr<CompositionBlueprintDocument> compositionBlueprintDocument = std::make_shared<CompositionBlueprintDocument>(resourceName.c_str(), ICON_MDI_VOLUME_HIGH, Document::Type::SoundDefinitions, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(compositionBlueprintDocument->GetCompositionBlueprint());
        resourceDocument = std::static_pointer_cast<Document>(compositionBlueprintDocument);
    }
    else if (resourceInfo.type == "PKGL")
    {
        std::shared_ptr<PackageListDocument> packageListDocument = std::make_shared<PackageListDocument>(resourceName.c_str(), ICON_MDI_VIEW_LIST, Document::Type::SoundDefinitions, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(packageListDocument->GetPackageList());
        resourceDocument = std::static_pointer_cast<Document>(packageListDocument);
    }
    else if (resourceInfo.type == "SCDA")
    {
        std::shared_ptr<ScatterDataDocument> scatterDataDocument = std::make_shared<ScatterDataDocument>(resourceName.c_str(), ICON_MDI_GRASS, Document::Type::SoundDefinitions, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(scatterDataDocument->GetScatterData());
        resourceDocument = std::static_pointer_cast<Document>(scatterDataDocument);
    }
    else if (resourceInfo.type == "CLOT")
    {
        std::shared_ptr<FabricResourceEntityDocument> fabricResourceEntityDocument = std::make_shared<FabricResourceEntityDocument>(resourceName.c_str(), ICON_MDI_TSHIRT_CREW, Document::Type::SoundDefinitions, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(fabricResourceEntityDocument->GetFabricResourceEntity());
        resourceDocument = std::static_pointer_cast<Document>(fabricResourceEntityDocument);
    }
    else if (resourceInfo.type == "PRIM")
    {
        std::shared_ptr<RenderPrimitiveDocument> renderPrimitiveDocument = std::make_shared<RenderPrimitiveDocument>(resourceName.c_str(), ICON_MDI_SHAPE, Document::Type::SoundDefinitions, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(renderPrimitiveDocument->GetRenderPrimitive());
        resourceDocument = std::static_pointer_cast<Document>(renderPrimitiveDocument);
    }
    else if (resourceInfo.type == "CLOS")
    {
        std::shared_ptr<ClothDocument> clothDocument = std::make_shared<ClothDocument>(resourceName.c_str(), ICON_MDI_SHAPE, Document::Type::SoundDefinitions, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(clothDocument->GetCloth());
        resourceDocument = std::static_pointer_cast<Document>(clothDocument);
    }
    else if (resourceInfo.type == "BORG")
    {
        std::shared_ptr<BoneRigDocument> boneRigDocument = std::make_shared<BoneRigDocument>(resourceName.c_str(), ICON_MDI_SHAPE, Document::Type::SoundDefinitions, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(boneRigDocument->GetBoneRig());
        resourceDocument = std::static_pointer_cast<Document>(boneRigDocument);
    }
    else if (resourceInfo.type == "ALOC")
    {
        std::shared_ptr<PhysicsDocument> physicsDocument = std::make_shared<PhysicsDocument>(resourceName.c_str(), ICON_MDI_SHAPE, Document::Type::SoundDefinitions, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(physicsDocument->GetPhysics());
        resourceDocument = std::static_pointer_cast<Document>(physicsDocument);
    }

    resource->SetHash(resourceInfo.hash);
    resource->SetResourceID(resourceInfo.resourceID);
    resource->SetHeaderLibraries(&resourceInfo.headerLibraries);
    resource->SetName(resourceName);

    Editor::GetInstance().GetDocuments().push_back(resourceDocument);

    std::thread thread(&ResourceBrowserPanel::LoadResource, this, resource, resourceNode, true);

    thread.detach();
}

void ResourceBrowserPanel::RenderFolderContextMenu(ResourceNode& folderNode, const std::string& folderPath)
{
    // Check if this folder has any TELI children
    const auto& resourcesInfo = ResourceInfoRegistry::GetInstance().GetResourcesInfo();
    bool hasTeli = false;
    std::set<std::string> languageSuffixes;

    for (const auto& [hash, info] : resourcesInfo)
    {
        if (info.type != "TELI")
        {
            continue;
        }

        if (info.resourceID.starts_with("[" + folderPath) || info.resourceID.starts_with("[[" + folderPath))
        {
            hasTeli = true;

            // Extract language suffix from resource name (e.g., _en, _pl, _ru)
            std::string resName = ResourceUtility::GetResourceName(info.resourceID);
            std::string lowerName = StringUtility::ToLowerCase(resName);

            // Find last underscore followed by 2 letters
            size_t lastUnderscore = lowerName.rfind('_');

            if (lastUnderscore != std::string::npos)
            {
                std::string suffix = lowerName.substr(lastUnderscore + 1);

                // Remove any trailing characters after the language code (e.g., file extensions in the name)
                if (suffix.length() >= 2)
                {
                    std::string langCode = suffix.substr(0, 2);

                    // Check it's alphabetic
                    if (std::isalpha(langCode[0]) && std::isalpha(langCode[1]))
                    {
                        languageSuffixes.insert("_" + langCode);
                    }
                }
            }
        }
    }

    if (!hasTeli)
    {
        ImGui::TextDisabled("No TELI files in this folder");
        return;
    }

    static std::string exportLabel = std::format("{} Export All TELI Files", ICON_MDI_EXPORT);
    static std::string importLabel = std::format("{} Patch Back To Game (Import All JSON Files)", ICON_MDI_IMPORT);

    if (ImGui::MenuItem(exportLabel.c_str()))
    {
        batchTeliParentPath = folderPath;
        showBatchTeliExportPopup = true;

        // Populate detected languages
        detectedLanguages.clear();
        selectedLanguages.clear();

        for (const auto& lang : languageSuffixes)
        {
            detectedLanguages.push_back(lang);
            selectedLanguages.push_back(true);
        }

        // If no language suffixes detected, add a default "all" option
        if (detectedLanguages.empty())
        {
            detectedLanguages.push_back("(all files - no language suffix detected)");
            selectedLanguages.push_back(true);
        }
    }

    if (ImGui::MenuItem(importLabel.c_str()))
    {
        batchTeliParentPath = folderPath;
        showBatchTeliImportPopup = true;
    }
}

void ResourceBrowserPanel::CollectTeliChildren(const std::string& folderPath,
    const std::vector<std::string>& languageFilters,
    std::vector<unsigned long long>& outHashes,
    std::vector<std::string>& outNames)
{
    const auto& resourcesInfo = ResourceInfoRegistry::GetInstance().GetResourcesInfo();

    for (const auto& [hash, info] : resourcesInfo)
    {
        if (info.type != "TELI")
        {
            continue;
        }

        if (!info.resourceID.starts_with("[" + folderPath) && !info.resourceID.starts_with("[[" + folderPath))
        {
            continue;
        }

        std::string resName = ResourceUtility::GetResourceName(info.resourceID);
        std::string lowerName = StringUtility::ToLowerCase(resName);

        // Apply language filter
        if (!languageFilters.empty())
        {
            bool hasDefaultAll = (languageFilters.size() == 1 && languageFilters[0].starts_with("(all"));

            if (!hasDefaultAll)
            {
                bool matchesLanguage = false;

                for (const auto& lang : languageFilters)
                {
                    if (lowerName.contains(lang))
                    {
                        matchesLanguage = true;
                        break;
                    }
                }

                if (!matchesLanguage)
                {
                    continue;
                }
            }
        }

        outHashes.push_back(hash);
        outNames.push_back(resName);
    }
}

void ResourceBrowserPanel::BatchExportTeliFiles(const std::string& outputFolder, const std::string& exportFormat)
{
    Logger::GetInstance().Log(Logger::Level::Info, std::format("Batch exporting {} TELI files to {}", batchTeliHashes.size(), outputFolder));

    unsigned int exportedCount = 0;

    for (size_t i = 0; i < batchTeliHashes.size(); ++i)
    {
        try
        {
            const ResourceInfoRegistry::ResourceInfo& resInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(batchTeliHashes[i]);

            std::shared_ptr<Resource> teliResource = ResourceUtility::CreateResource(resInfo.type);
            std::string resName = batchTeliNames[i];

            teliResource->SetHash(resInfo.hash);
            teliResource->SetResourceID(resInfo.resourceID);
            teliResource->SetHeaderLibraries(&resInfo.headerLibraries);
            teliResource->SetName(resName);

            // Load resource synchronously without references to avoid OOM / freeze
            if (resInfo.headerLibraries.size() > 0)
            {
                teliResource->LoadResource(0, resInfo.headerLibraries[0].chunkIndex, resInfo.headerLibraries[0].indexInLibrary, false, false, true);
            }
            else
            {
                teliResource->LoadResource(0, -1, -1, false, false, true);
            }

            if (!teliResource->IsResourceLoaded())
            {
                Logger::GetInstance().Log(Logger::Level::Error, std::format("Failed to load TELI resource: {}", resName));
                continue;
            }

        // Extract relative folder path to maintain directory structure
        std::string resID = resInfo.resourceID;
        std::string lowerResID = StringUtility::ToLowerCase(resID);
        std::string lowerParentPath = StringUtility::ToLowerCase(batchTeliParentPath);
        
        size_t pos = lowerResID.find(lowerParentPath);
        std::string relativePath = "";

        if (pos != std::string::npos)
        {
            size_t start = pos + batchTeliParentPath.length();
            size_t end = resID.find_last_of('/');
            if (end != std::string::npos && end > start)
            {
                relativePath = resID.substr(start, end - start);
            }
        }

        if (relativePath.starts_with("/"))
            relativePath = relativePath.substr(1);

            std::replace(relativePath.begin(), relativePath.end(), '/', '\\');

            // Sanitize path (remove invalid windows path characters)
            std::string invalidChars = "<>:\"|?*";
            for (char c : invalidChars)
            {
                std::replace(relativePath.begin(), relativePath.end(), c, '_');
                std::replace(resName.begin(), resName.end(), c, '_');
            }

            std::string finalOutputFolder = outputFolder;
            if (!relativePath.empty())
            {
                finalOutputFolder = std::format("{}\\{}", outputFolder, relativePath);
            }

            if (!std::filesystem::exists(finalOutputFolder))
            {
                std::filesystem::create_directories(finalOutputFolder);
            }

            std::string outputPath;

            if (exportFormat == "Json")
            {
                teliResource->Deserialize();
                outputPath = std::format("{}\\{}.TEXTLIST.JSON", finalOutputFolder, resName);
                teliResource->Export(outputPath, "Json File (.TEXTLIST.JSON)");
            }
            else
            {
                outputPath = std::format("{}\\{}.TELI", finalOutputFolder, resName);
                teliResource->Export(outputPath, "Raw File (.TELI)");
            }

            Logger::GetInstance().Log(Logger::Level::Info, std::format("Exported: {}", outputPath));
            exportedCount++;
        }
        catch (const std::exception& e)
        {
            Logger::GetInstance().Log(Logger::Level::Error, std::format("Exception during export of file {}: {}", batchTeliNames[i], e.what()));
        }
        catch (...)
        {
            Logger::GetInstance().Log(Logger::Level::Error, std::format("Unknown exception during export of file {}", batchTeliNames[i]));
        }
    }

    Logger::GetInstance().Log(Logger::Level::Info, std::format("Batch export complete: {}/{} files exported", exportedCount, batchTeliHashes.size()));
}

void ResourceBrowserPanel::BatchImportTeliFiles(const std::string& inputFolder)
{
    Logger::GetInstance().Log(Logger::Level::Info, std::format("Batch importing TELI files from {}", inputFolder));

    // Scan for JSON files in the input folder (recursively)
    std::vector<std::string> jsonFiles;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(inputFolder))
    {
        if (entry.is_regular_file())
        {
            std::string ext = entry.path().extension().string();
            std::string lowerExt = StringUtility::ToLowerCase(ext);

            if (lowerExt == ".json")
            {
                jsonFiles.push_back(entry.path().string());
            }
        }
    }

    if (jsonFiles.empty())
    {
        Logger::GetInstance().Log(Logger::Level::Warning, "No JSON files found in selected folder");
        return;
    }

    Logger::GetInstance().Log(Logger::Level::Info, std::format("Found {} JSON files", jsonFiles.size()));

    // Collect all TELI resources under this folder path (no language filter)
    std::vector<unsigned long long> teliHashes;
    std::vector<std::string> teliNames;
    std::vector<std::string> noFilter;
    CollectTeliChildren(batchTeliParentPath, noFilter, teliHashes, teliNames);

    unsigned int patchedCount = 0;

    for (const auto& jsonFile : jsonFiles)
    {
        std::string jsonFileName = std::filesystem::path(jsonFile).stem().string();

        // Remove .TEXTLIST suffix if present
        std::string lowerJsonName = StringUtility::ToLowerCase(jsonFileName);

        if (lowerJsonName.ends_with(".textlist"))
        {
            jsonFileName = jsonFileName.substr(0, jsonFileName.length() - 9); // remove ".TEXTLIST"
        }

        // Find matching TELI resource
        bool found = false;

        for (size_t i = 0; i < teliHashes.size(); ++i)
        {
            if (teliNames[i] == jsonFileName)
            {
                try
                {
                    const ResourceInfoRegistry::ResourceInfo& resInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(teliHashes[i]);

                    std::shared_ptr<Resource> teliResource = ResourceUtility::CreateResource(resInfo.type);

                    teliResource->SetHash(resInfo.hash);
                    teliResource->SetResourceID(resInfo.resourceID);
                    teliResource->SetHeaderLibraries(&resInfo.headerLibraries);
                    teliResource->SetName(teliNames[i]);

                    // Load resource synchronously without references to avoid OOM / freeze
                    if (resInfo.headerLibraries.size() > 0)
                    {
                        teliResource->LoadResource(0, resInfo.headerLibraries[0].chunkIndex, resInfo.headerLibraries[0].indexInLibrary, false, false, true);
                    }
                    else
                    {
                        teliResource->LoadResource(0, -1, -1, false, false, true);
                    }

                    if (!teliResource->IsResourceLoaded())
                    {
                        Logger::GetInstance().Log(Logger::Level::Error, std::format("Failed to load TELI resource: {}", teliNames[i]));
                        continue;
                    }

                    teliResource->Deserialize();

                    std::shared_ptr<TextList> textList = std::static_pointer_cast<TextList>(teliResource);
                    textList->ImportFromJson(jsonFile);
                    textList->PatchResourceLibrary();

                    Logger::GetInstance().Log(Logger::Level::Info, std::format("Patched: {} from {}", teliNames[i], jsonFile));
                    patchedCount++;
                    found = true;
                }
                catch (const std::exception& e)
                {
                    Logger::GetInstance().Log(Logger::Level::Error, std::format("Exception during import of file {}: {}", jsonFile, e.what()));
                }
                catch (...)
                {
                    Logger::GetInstance().Log(Logger::Level::Error, std::format("Unknown exception during import of file {}", jsonFile));
                }
                break;
            }
        }

        if (!found)
        {
            Logger::GetInstance().Log(Logger::Level::Warning, std::format("No matching TELI resource found for: {}", jsonFileName));
        }
    }

    Logger::GetInstance().Log(Logger::Level::Info, std::format("Batch import complete: {}/{} files patched", patchedCount, jsonFiles.size()));
}
