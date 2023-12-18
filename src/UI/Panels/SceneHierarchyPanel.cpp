#include <format>

#include <IconsMaterialDesignIcons.h>

#include "Utility/UI.h"
#include "UI/Panels/SceneHierarchyPanel.h"
#include "Utility/StringUtility.h"
#include "Rendering/DirectXRenderer.h"
#include "Connection/PipeClient.h"
#include "Registry/ResourceInfoRegistry.h"
#include "Editor.h"
#include "Resources/AspectEntityType.h"
#include "Resources/AspectEntityBlueprint.h"
#include "Utility/ResourceUtility.h"

SceneHierarchyPanel::EntityTreeNode::EntityTreeNode()
{
    entityIndex = -1;
    hasChildren = true;
}

SceneHierarchyPanel::EntityTreeNode::EntityTreeNode(const unsigned int entityIndex, const char* entityName) : entityIndex(entityIndex), entityName(entityName)
{
    hasChildren = true;
}

const bool SceneHierarchyPanel::EntityTreeNode::IsParent(std::shared_ptr<EntityTreeNode> entityTreeNode) const
{
    std::shared_ptr<EntityTreeNode> currentNode = parentNode.lock();

	while (currentNode)
	{
		if (currentNode.get() == entityTreeNode.get())
		{
			return true;
		}

        currentNode = currentNode->parentNode.lock();
	}

	return false;
}

const bool SceneHierarchyPanel::EntityTreeNode::HasInterface(const std::string& typeName) const
{
    for (size_t i = 0; i < blueprintResources.size(); ++i)
    {
        if (blueprintResources[i]->GetResourceHeaderHeader().m_type == 'TBLU')
        {
            std::shared_ptr<TemplateEntityBlueprint> tbluResource = std::static_pointer_cast<TemplateEntityBlueprint>(blueprintResources[i]);
            const int rootEntityIndex = tbluResource->GetTemplateEntityBlueprint()->rootEntityIndex;

            if (HasInterface(typeName, tbluResource, rootEntityIndex))
            {
                return true;
            }
        }
        else
        {
            std::shared_ptr<AspectEntityBlueprint> asebResource = std::static_pointer_cast<AspectEntityBlueprint>(blueprintResources[i]);
            const std::vector<std::shared_ptr<Resource>>& asebReferences = asebResource->GetReferences();

            for (size_t j = 0; j < asebReferences.size(); ++j)
            {
                std::shared_ptr<TemplateEntityBlueprint> tbluResource = std::static_pointer_cast<TemplateEntityBlueprint>(asebReferences[j]);
                const int rootEntityIndex = tbluResource->GetTemplateEntityBlueprint()->rootEntityIndex;

                if (HasInterface(typeName, tbluResource, rootEntityIndex))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

const bool SceneHierarchyPanel::EntityTreeNode::HasInterface(const std::string& typeName, std::shared_ptr<TemplateEntityBlueprint> tbluResource, const unsigned int entityIndex) const
{
    const STemplateSubEntityBlueprint& templateSubEntityBlueprint = tbluResource->GetTemplateEntityBlueprint()->entityTemplates[entityIndex];

    for (size_t i = 0; i < templateSubEntityBlueprint.exposedInterfaces.Size(); ++i)
    {
        const char* typeName2 = templateSubEntityBlueprint.exposedInterfaces[i].Key().ToCString();

        if (strcmp(typeName.c_str(), typeName2) == 0)
        {
            return true;
        }
    }

    return false;
}

SceneHierarchyPanel::SceneHierarchyPanel(const char* name, const char* icon, std::shared_ptr<TemplateEntity> tempResource) : BasePanel(name, icon)
{
    this->tempResource = tempResource;
    isEntityTreeGenerated = false;
    rootNode = std::make_shared<EntityTreeNode>();
    scrollToEntity = false;
    showNewEntityModal = false;
    showRenameEntityModal = false;
}

void SceneHierarchyPanel::Render()
{
    if (!Begin())
    {
        return;
    }

    ImGui::PushFont(Editor::GetInstance().GetImGuiRenderer()->GetMiddleFont());

    RenderEntityTree();
    RenderNewEntityModal();
    RenderRenameEntityModal();

    ImGui::PopFont();

    End();
}

void SceneHierarchyPanel::SetSelectedEntityCallback(SelectedEntityCallback selectedEntityCallback)
{
    this->selectedEntityCallback = selectedEntityCallback;
}

void SceneHierarchyPanel::OnGoToEntity(const std::shared_ptr<Resource> templateResource, const unsigned int entityIndex)
{
    selectedEntityTreeNode = FindNode(templateResource, entityIndex, rootNode);
    scrollToEntity = true;

    selectedEntityCallback(selectedEntityTreeNode);
}

void SceneHierarchyPanel::RenderEntityTree()
{
    if (!open || !rootNode)
    {
        return;
    }

    if (!isEntityTreeGenerated)
    {
        if (ImGui::Button("Generate Entity Tree"))
        {
            tempResource->Deserialize();
            tempResource->DeleteResourceData();

            std::vector<std::shared_ptr<Resource>>& tempReferences = tempResource->GetReferences();
            std::shared_ptr<TemplateEntityBlueprint> tbluResource = std::static_pointer_cast<TemplateEntityBlueprint>(tempReferences[tempReferences.size() - 1]);

            LoadAndDeserializeTBLU(tbluResource);

            rootNode->entityIndex = tempResource->GetTemplateEntity()->rootEntityIndex;
            rootNode->entityName = tbluResource->GetTemplateEntityBlueprint()->entityTemplates[rootNode->entityIndex].entityName.ToCString();

            rootNode->templateResources.push_back(tempResource);
            rootNode->blueprintResources.push_back(tbluResource);

            AddChildren(rootNode, tempResource, tbluResource, rootNode->entityIndex);

            const unsigned int templateReferenceIndex = tempResource->GetTemplateEntity()->entityTemplates[rootNode->entityIndex].entityTypeResourceIndex;
            const unsigned int blueprintReferenceIndex = tbluResource->GetTemplateEntityBlueprint()->entityTemplates[rootNode->entityIndex].entityTypeResourceIndex;
            const std::string& referenceType = GetTEMPReferenceType(tempResource, templateReferenceIndex);

            if (referenceType == "TEMP")
            {
                AddChildrenFromReferencedTEMP(rootNode, tempResource, tbluResource, templateReferenceIndex, blueprintReferenceIndex);
            }
            else if (referenceType == "ASET")
            {
                AddChildrenFromReferencedASET(rootNode, tempResource, tbluResource, templateReferenceIndex, blueprintReferenceIndex);
            }

            rootNode->templateResourceID = GetTEMPReferenceResourceID(tempResource, templateReferenceIndex);
            rootNode->blueprintResourceID = GetTBLUReferenceResourceID(tbluResource, blueprintReferenceIndex);

            isEntityTreeGenerated = true;
        }
    }
    else
    {
        static char name[256]{ "" };
        static std::string hint = std::format("{} Name...", ICON_MDI_MAGNIFY);

        ImGui::PushItemWidth(-1);
        ImGui::InputTextWithHint("##Name", hint.c_str(), name, IM_ARRAYSIZE(name));
        ImGui::PopItemWidth();

        ImGui::Spacing();

        if (ImGui::Button("Search Entity Name"))
        {
            if (strlen(name) == 0)
            {
                filteredTreeRootNode.reset();
            }
            else
            {
                std::unordered_map<std::shared_ptr<EntityTreeNode>, std::shared_ptr<EntityTreeNode>> parentMap;

                SearchEntityNameInTree(rootNode, name, parentMap);

                filteredTreeRootNode = GeneratedFilteredEntityTree(parentMap);
            }
        }

        if (ImGui::Button("Search Type Name"))
        {
            if (strlen(name) == 0)
            {
                filteredTreeRootNode.reset();
            }
            else
            {
                std::unordered_map<std::shared_ptr<EntityTreeNode>, std::shared_ptr<EntityTreeNode>> parentMap;

                SearchTypeNameInTree(rootNode, name, parentMap);

                filteredTreeRootNode = GeneratedFilteredEntityTree(parentMap);
            }
        }

        ImGui::Spacing();

        if (filteredTreeRootNode)
        {
            RenderEntityTree(filteredTreeRootNode, true);
        }
        else
        {
            RenderEntityTree(rootNode, false);
        }
    }
}

void SceneHierarchyPanel::RenderEntityTree(std::shared_ptr<EntityTreeNode> entityTreeNode, const bool isTreeFiltered)
{
    ImGui::PushID(entityTreeNode.get());

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_SpanFullWidth;
    const bool isNodeSelected = selectedEntityTreeNode && entityTreeNode == selectedEntityTreeNode;
    const bool isExternalEntity = entityTreeNode->templateResources[0] != tempResource;

    if (isNodeSelected)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;

        if (scrollToEntity)
        {
            ImGui::SetScrollHereY();

            scrollToEntity = false;
        }
    }
    else if (scrollToEntity)
    {
        if (selectedEntityTreeNode->IsParent(entityTreeNode))
        {
            ImGui::SetNextItemOpen(true);
        }
    }
    else if (isTreeFiltered)
    {
        ImGui::SetNextItemOpen(true);
    }

    if (entityTreeNode->hasChildren)
    {
        nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;

        if (isExternalEntity)
        {
            ImGui::PushFont(Editor::GetInstance().GetImGuiRenderer()->GetMiddleItalicFont());
        }

        const bool isNodeOpen = ImGui::TreeNodeEx(entityTreeNode->entityName.c_str(), nodeFlags);

        if (isExternalEntity)
        {
            ImGui::PopFont();
        }

        if ((ImGui::IsItemClicked() || ImGui::IsItemClicked(ImGuiPopupFlags_MouseButtonRight)) && !ImGui::IsItemToggledOpen())
        {
            selectedEntityTreeNode = entityTreeNode;

            selectedEntityCallback(selectedEntityTreeNode);
        }

        if (ImGui::BeginPopupContextItem())
        {
            RenderContextMenu(entityTreeNode);

            ImGui::EndPopup();
        }

        if (isNodeOpen)
        {
            for (std::shared_ptr<EntityTreeNode> entityTreeNode : entityTreeNode->children)
            {
                RenderEntityTree(entityTreeNode, isTreeFiltered);
            }

            ImGui::TreePop();
        }
    }
    else
    {
        nodeFlags |= ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf;

        if (isExternalEntity)
        {
            ImGui::PushFont(Editor::GetInstance().GetImGuiRenderer()->GetMiddleItalicFont());
        }

        ImGui::TreeNodeEx(entityTreeNode->entityName.c_str(), nodeFlags);

        if (isExternalEntity)
        {
            ImGui::PopFont();
        }

        if ((ImGui::IsItemClicked() || ImGui::IsItemClicked(ImGuiPopupFlags_MouseButtonRight)) && !ImGui::IsItemToggledOpen())
        {
            selectedEntityTreeNode = entityTreeNode;

            selectedEntityCallback(selectedEntityTreeNode);
        }

        if (ImGui::BeginPopupContextItem())
        {
            RenderContextMenu(entityTreeNode);

            ImGui::EndPopup();
        }
    }

    ImGui::PopID();
}

void SceneHierarchyPanel::RenderContextMenu(std::shared_ptr<EntityTreeNode> entityTreeNode)
{
    static std::string addNewChildLabel = std::format("{} Add New Child", ICON_MDI_PLUS);
    static std::string deleteEntityLabel = std::format("{} Delete Entity", ICON_MDI_CLOSE);
    static std::string renameEntityLabel = std::format("{} Rename Entity", ICON_MDI_CLOSE);

    if (ImGui::MenuItem(addNewChildLabel.c_str()))
    {
        showNewEntityModal = true;
    }

    if (ImGui::MenuItem(deleteEntityLabel.c_str()))
    {
        std::shared_ptr<EntityTreeNode> parentNode = entityTreeNode->parentNode.lock();

        for (auto it = parentNode->children.begin(); it != parentNode->children.end(); ++it)
        {
            if (*it == entityTreeNode)
            {
                parentNode->children.erase(it);

                break;
            }
        }
    }

    if (ImGui::MenuItem(renameEntityLabel.c_str()))
    {
        showRenameEntityModal = true;
    }
}

void SceneHierarchyPanel::RenderNewEntityModal()
{
    if (showNewEntityModal)
    {
        ImGui::OpenPopup("Add New Child");
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();

    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Add New Child", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char entityName[100]{ "" };
        static char templateResourceID[512]{ "" };
        static char blueprintResourceID[512]{ "" };

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Entity Name");
        ImGui::SameLine();

        ImGui::InputText("##EntityName", entityName, sizeof(entityName));

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Template");
        ImGui::SameLine();

        ImGui::InputText("##Template", templateResourceID, sizeof(templateResourceID));

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Blueprint");
        ImGui::SameLine();

        ImGui::InputText("##Blueprint", blueprintResourceID, sizeof(blueprintResourceID));

        ImGui::Spacing();
        ImGui::Spacing();

        if (UI::IconButton("  " ICON_MDI_CHECK, " Ok "))
        {
            showNewEntityModal = false;

            AddNewEntity(entityName, templateResourceID, blueprintResourceID, selectedEntityTreeNode);

            ImGui::CloseCurrentPopup();
        }

        const ImVec2 framePadding = ImGui::GetStyle().FramePadding;

        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + framePadding.x);

        if (UI::IconButton("  " ICON_MDI_CLOSE, " Cancel "))
        {
            showNewEntityModal = false;

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void SceneHierarchyPanel::RenderRenameEntityModal()
{
    if (showRenameEntityModal)
    {
        ImGui::OpenPopup("Rename Entity");
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();

    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Rename Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Entity Name");
        ImGui::SameLine();

        ImGui::InputText("##EntityName", &selectedEntityTreeNode->entityName);

        ImGui::Spacing();
        ImGui::Spacing();

        if (UI::IconButton("  " ICON_MDI_CHECK, " Ok "))
        {
            showRenameEntityModal = false;

            ImGui::CloseCurrentPopup();
        }

        const ImVec2 framePadding = ImGui::GetStyle().FramePadding;

        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + framePadding.x);

        if (UI::IconButton("  " ICON_MDI_CLOSE, " Cancel "))
        {
            showRenameEntityModal = false;

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void SceneHierarchyPanel::AddChildren(std::shared_ptr<EntityTreeNode> entityTreeNode, std::shared_ptr<TemplateEntity> tempResource, std::shared_ptr<TemplateEntityBlueprint> tbluResource, const int parentIndex, const bool addTempReferenceToTemplateResources)
{
    std::shared_ptr<STemplateEntity> templateEntity = tempResource->GetTemplateEntity();
    std::shared_ptr<STemplateEntityBlueprint> templateEntityBlueprint = tbluResource->GetTemplateEntityBlueprint();

    for (size_t i = 0; i < templateEntityBlueprint->entityTemplates.Size(); ++i)
    {
        if (templateEntityBlueprint->entityTemplates[i].parentIndex == parentIndex)
        {
            std::shared_ptr<EntityTreeNode> childNode = std::make_shared<EntityTreeNode>(i, templateEntityBlueprint->entityTemplates[i].entityName.ToCString());

            childNode->parentNode = entityTreeNode;

            childNode->templateResources.push_back(tempResource);
            childNode->blueprintResources.push_back(tbluResource);

            AddChildren(childNode, tempResource, tbluResource, childNode->entityIndex);

            const unsigned int templateReferenceIndex = templateEntity->entityTemplates[i].entityTypeResourceIndex;
            const unsigned int blueprintReferenceIndex = templateEntityBlueprint->entityTemplates[i].entityTypeResourceIndex;
            const std::string& referenceType = GetTEMPReferenceType(tempResource, templateReferenceIndex);

            if (referenceType == "TEMP")
            {
                AddChildrenFromReferencedTEMP(childNode, tempResource, tbluResource, templateReferenceIndex, blueprintReferenceIndex);
            }
            else if (referenceType == "ASET")
            {
                AddChildrenFromReferencedASET(childNode, tempResource, tbluResource, templateReferenceIndex, blueprintReferenceIndex);
            }

            childNode->templateResourceID = GetTEMPReferenceResourceID(tempResource, templateReferenceIndex);
            childNode->blueprintResourceID = GetTBLUReferenceResourceID(tbluResource, blueprintReferenceIndex);

            if (childNode->children.size() > 0)
            {
                childNode->hasChildren = true;
            }

            entityTreeNode->children.push_back(childNode);
        }
    }

    if (entityTreeNode->children.size() > 0)
    {
        std::sort(entityTreeNode->children.begin(), entityTreeNode->children.end(), [](const std::shared_ptr<EntityTreeNode> a, const std::shared_ptr<EntityTreeNode> b)
        {
            return a->entityName < b->entityName;
        });
    }
    else
    {
        entityTreeNode->hasChildren = false;
    }
}

void SceneHierarchyPanel::AddChildrenFromReferencedTEMP(std::shared_ptr<EntityTreeNode> childNode, std::shared_ptr<TemplateEntity> tempResource, std::shared_ptr<TemplateEntityBlueprint> tbluResource, const unsigned int templateReferenceIndex, const unsigned int blueprintReferenceIndex)
{
    const std::vector<std::shared_ptr<Resource>>& tempReferences = tempResource->GetReferences();
    const std::vector<std::shared_ptr<Resource>>& tbluReferences = tbluResource->GetReferences();
    std::shared_ptr<TemplateEntity> tempReference = std::static_pointer_cast<TemplateEntity>(tempReferences[templateReferenceIndex]);
    std::shared_ptr<TemplateEntityBlueprint> tbluReference = std::static_pointer_cast<TemplateEntityBlueprint>(tbluReferences[blueprintReferenceIndex]);

    AddChildrenFromReferencedTEMP(childNode, tempReference, tbluReference);
}

void SceneHierarchyPanel::AddChildrenFromReferencedTEMP(std::shared_ptr<EntityTreeNode> childNode, std::shared_ptr<TemplateEntity> tempReference, std::shared_ptr<TemplateEntityBlueprint> tbluReference, const bool addTempReferenceToTemplateResources)
{
    if (!tempReference->GetTemplateEntity())
    {
        LoadAndDeserializeTEMP(tempReference);
    }

    if (!tbluReference->GetTemplateEntityBlueprint())
    {
        LoadAndDeserializeTBLU(tbluReference);
    }

    const unsigned int rootEntityIndex = tempReference->GetTemplateEntity()->rootEntityIndex;

    if (addTempReferenceToTemplateResources)
    {
        childNode->templateResources.push_back(tempReference);
        childNode->blueprintResources.push_back(tbluReference);
    }

    AddChildren(childNode, tempReference, tbluReference, rootEntityIndex, true);

    const unsigned int templateReferenceIndex2 = tempReference->GetTemplateEntity()->entityTemplates[rootEntityIndex].entityTypeResourceIndex;
    const unsigned int blueprintReferenceIndex2 = tbluReference->GetTemplateEntityBlueprint()->entityTemplates[rootEntityIndex].entityTypeResourceIndex;
    const std::string& referenceType = GetTEMPReferenceType(tempReference, templateReferenceIndex2);

    if (referenceType == "TEMP")
    {
        AddChildrenFromReferencedTEMP(childNode, tempReference, tbluReference, templateReferenceIndex2, blueprintReferenceIndex2);
    }
    else if (referenceType == "ASET")
    {
        AddChildrenFromReferencedASET(childNode, tempReference, tbluReference, templateReferenceIndex2, blueprintReferenceIndex2);
    }
}

void SceneHierarchyPanel::AddChildrenFromReferencedASET(std::shared_ptr<EntityTreeNode> childNode, std::shared_ptr<TemplateEntity> tempResource, std::shared_ptr<TemplateEntityBlueprint> tbluResource, const unsigned int templateReferenceIndex, const unsigned int blueprintReferenceIndex)
{
    const std::vector<std::shared_ptr<Resource>>& tempReferences = tempResource->GetReferences();
    const std::vector<std::shared_ptr<Resource>>& tbluReferences = tbluResource->GetReferences();
    std::shared_ptr<AspectEntityType> asetReference = std::static_pointer_cast<AspectEntityType>(tempReferences[templateReferenceIndex]);
    std::shared_ptr<AspectEntityBlueprint> asebReference = std::static_pointer_cast<AspectEntityBlueprint>(tbluReferences[blueprintReferenceIndex]);

    if (!asetReference->IsResourceLoaded())
    {
        const ResourceInfoRegistry::ResourceInfo& asetResourceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(asetReference->GetHash());

        asetReference->LoadResource(0, asetResourceInfo.headerLibraries[0].chunkIndex, asetResourceInfo.headerLibraries[0].indexInLibrary, true, false, true);
    }

    if (!asebReference->IsResourceLoaded())
    {
        const ResourceInfoRegistry::ResourceInfo& asebResourceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(asebReference->GetHash());

        asebReference->LoadResource(0, asebResourceInfo.headerLibraries[0].chunkIndex, asebResourceInfo.headerLibraries[0].indexInLibrary, true, false, true);
    }

    const std::vector<std::shared_ptr<Resource>>& asetReferences = asetReference->GetReferences();
    const std::vector<std::shared_ptr<Resource>>& asebReferences = asebReference->GetReferences();

    if (asetReferences[0]->GetResourceHeaderHeader().m_type == 'TEMP')
    {
        childNode->templateResources.push_back(asetReference);
        childNode->blueprintResources.push_back(asebReference);

        std::shared_ptr<TemplateEntity> tempReference = std::static_pointer_cast<TemplateEntity>(asetReferences[0]);
        std::shared_ptr<TemplateEntityBlueprint> tbluReference = std::static_pointer_cast<TemplateEntityBlueprint>(asebReferences[0]);

        AddChildrenFromReferencedTEMP(childNode, tempReference, tbluReference, false);

        for (size_t i = 1; i < asetReferences.size() - 1; ++i)
        {
            if (asetReferences[i]->GetResourceHeaderHeader().m_type == 'TEMP')
            {
                std::shared_ptr<TemplateEntity> reference = std::static_pointer_cast<TemplateEntity>(asetReferences[i]);

                if (!reference->GetTemplateEntity())
                {
                    LoadAndDeserializeTEMP(reference);
                }
            }
        }
    }
}

void SceneHierarchyPanel::AddNewEntity(const char* entityName, const char* templateResourceID, const char* blueprintResourceID, std::shared_ptr<EntityTreeNode> parentEntityTreeNode)
{
    ZRuntimeResourceID templateRuntimeResourceID = ZRuntimeResourceID::QueryRuntimeResourceID(templateResourceID);
    ZRuntimeResourceID blueprintRuntimeResourceID = ZRuntimeResourceID::QueryRuntimeResourceID(templateResourceID);

    std::shared_ptr<TemplateEntity> templateResource = std::static_pointer_cast<TemplateEntity>(parentEntityTreeNode->templateResources[0]);
    std::shared_ptr<TemplateEntityBlueprint> blueprintResource = std::static_pointer_cast<TemplateEntityBlueprint>(parentEntityTreeNode->blueprintResources[0]);
    const std::vector<std::shared_ptr<Resource>>& tempReferences = templateResource->GetReferences();
    const std::vector<std::shared_ptr<Resource>>& tbluReferences = blueprintResource->GetReferences();
    unsigned int templateReferenceIndex = -1;
    unsigned int blueprintReferenceIndex = -1;

    for (size_t i = 0; i < tempReferences.size(); ++i)
    {
        const std::shared_ptr<Resource> reference = tempReferences[i];

        if (reference->GetRuntimeResourceID() == templateRuntimeResourceID)
        {
            templateReferenceIndex = i;

            break;
        }
    }

    for (size_t i = 0; i < tbluReferences.size(); ++i)
    {
        const std::shared_ptr<Resource> reference = tbluReferences[i];

        if (reference->GetRuntimeResourceID() == blueprintRuntimeResourceID)
        {
            blueprintReferenceIndex = i;

            break;
        }
    }

    if (templateReferenceIndex == -1)
    {
        templateResource->AddReference(templateRuntimeResourceID, EResourceReferenceFlags::RESOURCE_REFERENCE_INSTALL_DEPENDENCY);

        templateReferenceIndex = tempReferences.size() - 1;
        //Load and deserialize if it's TEMP or ASET with TEMPs
    }

    if (blueprintReferenceIndex == -1)
    {
        templateResource->AddReference(blueprintRuntimeResourceID, EResourceReferenceFlags::RESOURCE_REFERENCE_INSTALL_DEPENDENCY);

        blueprintReferenceIndex = tbluReferences.size() - 1;
    }

    const unsigned int newEntityIndex = templateResource->GetTemplateEntity()->entityTemplates.Size();
    std::shared_ptr<EntityTreeNode> childNode = std::make_shared<EntityTreeNode>(newEntityIndex, entityName);

    childNode->parentNode = parentEntityTreeNode;
    parentEntityTreeNode->hasChildren = true;

    childNode->templateResources.push_back(parentEntityTreeNode->templateResources[0]);
    childNode->blueprintResources.push_back(parentEntityTreeNode->blueprintResources[0]);
    parentEntityTreeNode->children.push_back(childNode);

    STemplateSubEntity templateSubEntity;
    STemplateSubEntityBlueprint templateSubEntityBlueprint;

    templateSubEntity.parentIndex = parentEntityTreeNode->entityIndex;
    templateSubEntity.entityTypeResourceIndex = templateReferenceIndex;
    templateSubEntityBlueprint.parentIndex = parentEntityTreeNode->entityIndex;
    templateSubEntityBlueprint.entityTypeResourceIndex = blueprintReferenceIndex;
    templateSubEntityBlueprint.entityName = childNode->entityName.c_str();

    templateResource->GetTemplateEntity()->entityTemplates.PushBack(templateSubEntity);
    blueprintResource->GetTemplateEntityBlueprint()->entityTemplates.PushBack(templateSubEntityBlueprint);
}

std::shared_ptr<SceneHierarchyPanel::EntityTreeNode> SceneHierarchyPanel::FindNode(const std::shared_ptr<Resource> templateResource, const unsigned int entityIndex, std::shared_ptr<EntityTreeNode> entityTreeNode) const
{
    if (entityTreeNode->entityIndex == entityIndex && entityTreeNode->templateResources[0] == templateResource)
    {
        return entityTreeNode;
    }

    for (size_t i = 0; i < entityTreeNode->children.size(); ++i)
    {
        if (entityTreeNode->children[i]->entityIndex == entityIndex && entityTreeNode->children[i]->templateResources[0] == templateResource)
        {
            return entityTreeNode->children[i];
        }

        std::shared_ptr<EntityTreeNode> entityTreeNode2 = FindNode(templateResource, entityIndex, entityTreeNode->children[i]);

        if (entityTreeNode2)
        {
            return entityTreeNode2;
        }
    }

    return nullptr;
}

void SceneHierarchyPanel::SearchEntityNameInTree(std::shared_ptr<EntityTreeNode> node, const std::string& entityName, std::unordered_map<std::shared_ptr<EntityTreeNode>, std::shared_ptr<EntityTreeNode>>& parentMap)
{
    if (!node)
    {
        return;
    }

    std::string entityName2 = entityName;
    std::string entityName3 = node->entityName;

    std::transform(entityName2.begin(), entityName2.end(), entityName2.begin(), tolower);
    std::transform(entityName3.begin(), entityName3.end(), entityName3.begin(), tolower);

    if (entityName3.contains(entityName2))
    {
        parentMap[node] = node->parentNode.lock();
    }

    for (auto& child : node->children)
    {
        SearchEntityNameInTree(child, entityName, parentMap);
    }
}

void SceneHierarchyPanel::SearchTypeNameInTree(std::shared_ptr<EntityTreeNode> node, const std::string& typeName, std::unordered_map<std::shared_ptr<EntityTreeNode>, std::shared_ptr<EntityTreeNode>>& parentMap)
{
    if (!node)
    {
        return;
    }

    if (node->HasInterface(typeName))
    {
        parentMap[node] = node->parentNode.lock();
    }

    for (auto& child : node->children)
    {
        SearchTypeNameInTree(child, typeName, parentMap);
    }
}

std::shared_ptr<SceneHierarchyPanel::EntityTreeNode> SceneHierarchyPanel::GeneratedFilteredEntityTree(const std::unordered_map<std::shared_ptr<EntityTreeNode>, std::shared_ptr<EntityTreeNode>>& parentMap)
{
    std::shared_ptr<EntityTreeNode> searchRoot = std::make_shared<EntityTreeNode>();

    searchRoot->entityName = "Scene";

    std::unordered_map<std::shared_ptr<EntityTreeNode>, std::shared_ptr<EntityTreeNode>> searchTreeMap;

    for (const auto& pair : parentMap)
    {
        std::shared_ptr<EntityTreeNode> currentNode = pair.first;
        std::shared_ptr<EntityTreeNode> parentInOriginalTree = pair.second;
        std::shared_ptr<EntityTreeNode> parentNodeInSearchTree;

        if (parentInOriginalTree)
        {
            if (searchTreeMap.find(parentInOriginalTree) == searchTreeMap.end())
            {
                parentNodeInSearchTree = std::make_shared<EntityTreeNode>(parentInOriginalTree->entityIndex, parentInOriginalTree->entityName.c_str());

                searchTreeMap[parentInOriginalTree] = parentNodeInSearchTree;
                searchRoot->children.push_back(parentNodeInSearchTree);
            }
            else
            {
                parentNodeInSearchTree = searchTreeMap[parentInOriginalTree];
            }
        }
        else
        {
            parentNodeInSearchTree = searchRoot;
        }

        std::shared_ptr<EntityTreeNode> newNode = std::make_shared<EntityTreeNode>(currentNode->entityIndex, currentNode->entityName.c_str());

        parentNodeInSearchTree->children.push_back(newNode);
        searchTreeMap[currentNode] = newNode;
    }

    return searchRoot;
}

const std::string& SceneHierarchyPanel::GetTEMPReferenceType(std::shared_ptr<TemplateEntity> tempResource, const unsigned int referenceIndex)
{
    const std::vector<std::shared_ptr<Resource>>& tempReferences = tempResource->GetReferences();
    const std::shared_ptr<Resource> reference = tempReferences[referenceIndex];
    const ResourceInfoRegistry::ResourceInfo& referenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(reference->GetHash());

    return referenceInfo.type;
}

const std::string& SceneHierarchyPanel::GetTEMPReferenceResourceID(std::shared_ptr<TemplateEntity> tempResource, const unsigned int referenceIndex)
{
    const std::vector<std::shared_ptr<Resource>>& tempReferences = tempResource->GetReferences();
    const std::shared_ptr<Resource> reference = tempReferences[referenceIndex];
    const ResourceInfoRegistry::ResourceInfo& referenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(reference->GetHash());

    return referenceInfo.resourceID;
}

const std::string& SceneHierarchyPanel::GetTBLUReferenceResourceID(std::shared_ptr<TemplateEntityBlueprint> tbluResource, const unsigned int referenceIndex)
{
    const std::vector<std::shared_ptr<Resource>>& tbluReferences = tbluResource->GetReferences();
    const std::shared_ptr<Resource> reference = tbluReferences[referenceIndex];
    const ResourceInfoRegistry::ResourceInfo& referenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(reference->GetHash());

    return referenceInfo.resourceID;
}

void SceneHierarchyPanel::LoadAndDeserializeTEMP(std::shared_ptr<TemplateEntity> tempResource)
{
    const ResourceInfoRegistry::ResourceInfo& tempResourceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(tempResource->GetHash());

    tempResource->LoadResource(0, tempResourceInfo.headerLibraries[0].chunkIndex, tempResourceInfo.headerLibraries[0].indexInLibrary, true, false, true);
    tempResource->Deserialize();
    tempResource->DeleteResourceData();
}

void SceneHierarchyPanel::LoadAndDeserializeTBLU(std::shared_ptr<TemplateEntityBlueprint> tbluResource)
{
    const ResourceInfoRegistry::ResourceInfo& tbluResourceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(tbluResource->GetHash());

    tbluResource->LoadResource(0, tbluResourceInfo.headerLibraries[0].chunkIndex, tbluResourceInfo.headerLibraries[0].indexInLibrary, true, false, true);
    tbluResource->Deserialize();
    tbluResource->DeleteResourceData();
}

void SceneHierarchyPanel::OnReceiveMessage(const std::string& type, const std::string& content)
{

}
