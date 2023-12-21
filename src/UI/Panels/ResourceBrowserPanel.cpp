#include <format>
#include <fstream>

#include <IconsMaterialDesignIcons.h>

#include "UI/Panels/ResourceBrowserPanel.h"
#include "Utility/StringUtility.h"
#include "Logger.h"
#include "Utility/ResourceUtility.h"
#include "Utility/UI.h"
#include "UI/Panels/HexViewerPanel.h"
#include "UI/Panels/ResourceInfoPanel.h"
#include "UI/Panels/ResourceViewerPanel.h"
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
#include "UI/Documents/SoundBlendContainerExternalParametersTypeDocument.h"
#include "UI/Documents/SoundBlendContainerExternalParametersBlueprintDocument.h"
#include "Editor.h"

ResourceBrowserPanel::ResourceBrowserPanel(const char* name, const char* icon) : BasePanel(name, icon)
{
    isInputTextActive = false;
    selectedNodeIndex = -1;

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

        ImGui::BeginChild("ScrollingRegion", ImVec2(300, 300), false, ImGuiWindowFlags_HorizontalScrollbar);

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
    }

    ImGui::PopFont();

    End();
}

void ResourceBrowserPanel::RenderTree(ResourceNode& parentNode, std::string parentPath)
{
    ImGui::PushID(&parentNode);

    std::string label;
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_SpanFullWidth;
    const bool isNodeSelected = selectedNodeIndices.contains(parentNode.index);

    if (isNodeSelected)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
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

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            selectedNodeIndex = parentNode.index;
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
        {
            CreateResourceDocument(parentNode);
        }
    }

    ImGui::PopID();
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

        if (!resourceTypes[resourceInfo.type] || isInputTextActive && !resourceInfo.resourceID.contains(resourceName))
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

void ResourceBrowserPanel::LoadResource(std::shared_ptr<Resource> resource, const ResourceNode& resourceNode)
{
    const ResourceInfoRegistry::ResourceInfo& resourceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(resourceNode.hash);

    if (resourceInfo.headerLibraries.size() > 0)
    {
        resource->LoadResource(0, resourceInfo.headerLibraries[0].chunkIndex, resourceInfo.headerLibraries[0].indexInLibrary, true, true, true);
    }
    else
    {
        resource->LoadResource(0, -1, -1, false, false, true);
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
        std::shared_ptr<TextListDocument> textureDocument = std::make_shared<TextListDocument>(resourceName.c_str(), ICON_MDI_TEXT_BOX, Document::Type::Texture, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(textureDocument->GetTextList());
        resourceDocument = std::static_pointer_cast<Document>(textureDocument);
    }
    else if (resourceInfo.type == "LOCR")
    {
        std::shared_ptr<LocalizationDocument> localizationDocument = std::make_shared<LocalizationDocument>(resourceName.c_str(), ICON_MDI_TRANSLATE, Document::Type::Texture, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(localizationDocument->GetLocalization());
        resourceDocument = std::static_pointer_cast<Document>(localizationDocument);
    }
    else if (resourceInfo.type == "LOCM")
    {
        std::shared_ptr<MultiLanguageDocument> multiLanguageDocument = std::make_shared<MultiLanguageDocument>(resourceName.c_str(), ICON_MDI_TRANSLATE, Document::Type::Texture, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(multiLanguageDocument->GetMultiLanguage());
        resourceDocument = std::static_pointer_cast<Document>(multiLanguageDocument);
    }
    else if (resourceInfo.type == "MATT")
    {
        std::shared_ptr<RenderMaterialEntityTypeDocument> renderMaterialEntityTypeDocument = std::make_shared<RenderMaterialEntityTypeDocument>(resourceName.c_str(), ICON_MDI_PALETTE, Document::Type::Texture, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(renderMaterialEntityTypeDocument->GetRenderMaterialEntityType());
        resourceDocument = std::static_pointer_cast<Document>(renderMaterialEntityTypeDocument);
    }
    else if (resourceInfo.type == "MATI")
    {
        std::shared_ptr<RenderMaterialInstanceDocument> renderMaterialInstanceDocument = std::make_shared<RenderMaterialInstanceDocument>(resourceName.c_str(), ICON_MDI_PALETTE, Document::Type::Texture, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(renderMaterialInstanceDocument->GetRenderMaterialInstance());
        resourceDocument = std::static_pointer_cast<Document>(renderMaterialInstanceDocument);
    }
    else if (resourceInfo.type == "SBPD")
    {
        std::shared_ptr<SoundBlendContainerExternalParametersTypeDocument> soundBlendContainerExternalParametersTypeDocument = std::make_shared<SoundBlendContainerExternalParametersTypeDocument>(resourceName.c_str(), ICON_MDI_VOLUME_HIGH, Document::Type::Texture, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(soundBlendContainerExternalParametersTypeDocument->GetSoundBlendContainerExternalParametersType());
        resourceDocument = std::static_pointer_cast<Document>(soundBlendContainerExternalParametersTypeDocument);
    }
    else if (resourceInfo.type == "SBPB")
    {
        std::shared_ptr<SoundBlendContainerExternalParametersBlueprintDocument> soundBlendContainerExternalParametersBlueprintDocument = std::make_shared<SoundBlendContainerExternalParametersBlueprintDocument>(resourceName.c_str(), ICON_MDI_VOLUME_HIGH, Document::Type::Texture, resourceInfo.hash, false, defaultDockID);

        resource = std::static_pointer_cast<Resource>(soundBlendContainerExternalParametersBlueprintDocument->GetSoundBlendContainerExternalParametersBlueprint());
        resourceDocument = std::static_pointer_cast<Document>(soundBlendContainerExternalParametersBlueprintDocument);
    }
    else if (resourceInfo.type == "PRIM")
    {
        /*resource = std::make_shared<RenderPrimitive>();
        std::shared_ptr<RenderPrimitive> renderPrimitive = std::static_pointer_cast<RenderPrimitive>(resource);

        resourceDocument = std::make_shared<Document>(resourceName.c_str(), ICON_MDI_FILE_DOCUMENT, Document::Type::RenderPrimitive, true, false);

        resourceDocument->AddPanel(std::make_shared<SceneHierarchyPanel2>("Scene Hierarchy", ICON_MDI_VIEW_LIST, renderPrimitive));
        resourceDocument->AddPanel(std::make_shared<BoneHierarchyPanel>("Bone Hierarchy", ICON_MDI_VIEW_LIST, renderPrimitive));
        resourceDocument->AddPanel(std::make_shared<ComponentPropertiesPanel>("Properties", ICON_MDI_WRENCH));*/
    }

    resource->SetHash(resourceInfo.hash);
    resource->SetResourceID(resourceInfo.resourceID);
    resource->SetHeaderLibraries(&resourceInfo.headerLibraries);
    resource->SetName(resourceName);

    Editor::GetInstance().GetDocuments().push_back(resourceDocument);

    std::thread thread(&ResourceBrowserPanel::LoadResource, this, resource, resourceNode);

    thread.detach();
}
