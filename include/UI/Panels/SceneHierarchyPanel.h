#pragma once

#include <memory>
#include <vector>
#include <functional>

#include <Glacier/Resource/ZRuntimeResourceID.h>

#include "BasePanel.h"
#include "Resources/TemplateEntity.h"
#include "Resources/TemplateEntityBlueprint.h"

class SceneHierarchyPanel : public BasePanel
{
public:
    struct EntityTreeNode
    {
        EntityTreeNode();
        EntityTreeNode(const unsigned int entityIndex, const char* entityName);
        const bool IsParent(std::shared_ptr<EntityTreeNode> entityTreeNode) const;
        const bool HasInterface(const std::string& typeName) const;
        const bool HasInterface(const std::string& typeName, std::shared_ptr<TemplateEntityBlueprint> tbluResource, const unsigned int entityIndex) const;

        unsigned int entityIndex;
        std::weak_ptr<EntityTreeNode> parentNode;
        std::string entityName;
        std::vector<std::shared_ptr<EntityTreeNode>> children;
        bool hasChildren;
        std::vector<std::shared_ptr<Resource>> templateResources;
        std::vector<std::shared_ptr<Resource>> blueprintResources;
        std::string templateResourceID;
        std::string blueprintResourceID;
    };

    using SelectedEntityCallback = std::function<void(std::shared_ptr<EntityTreeNode> selectedEntityTreeNode)>;

    SceneHierarchyPanel(const char* name, const char* icon, std::shared_ptr<TemplateEntity> tempResource);
    void Render() override;
    void SetSelectedEntityCallback(SelectedEntityCallback selectedEntityCallback);
    void OnGoToEntity(const std::shared_ptr<Resource> templateResource, const unsigned int entityIndex);
    void OnReceiveMessage(const std::string& type, const std::string& content);

private:
    void RenderEntityTree();
    void RenderEntityTree(std::shared_ptr<EntityTreeNode> entityTreeNode, const bool isTreeFiltered);
    void RenderContextMenu(std::shared_ptr<EntityTreeNode> entityTreeNode);
    void RenderNewEntityModal();
    void RenderRenameEntityModal();

    void AddChildren(std::shared_ptr<EntityTreeNode> entityTreeNode, std::shared_ptr<TemplateEntity> tempResource, std::shared_ptr<TemplateEntityBlueprint> tbluResource, const int parentIndex = -1, const bool addTempReferenceToTemplateResources = false);
    void AddChildrenFromReferencedTEMP(std::shared_ptr<EntityTreeNode> childNode, std::shared_ptr<TemplateEntity> tempResource, std::shared_ptr<TemplateEntityBlueprint> tbluResource, const unsigned int templateReferenceIndex, const unsigned int blueprintReferenceIndex);
    void AddChildrenFromReferencedTEMP(std::shared_ptr<EntityTreeNode> childNode, std::shared_ptr<TemplateEntity> tempReference, std::shared_ptr<TemplateEntityBlueprint> tbluReference, const bool addTempReferenceToTemplateResources = true);
    void AddChildrenFromReferencedASET(std::shared_ptr<EntityTreeNode> childNode, std::shared_ptr<TemplateEntity> tempResource, std::shared_ptr<TemplateEntityBlueprint> tbluResource, const unsigned int templateReferenceIndex, const unsigned int blueprintReferenceIndex);
    void AddNewEntity(const char* entityName, const char* templateResourceID, const char* blueprintResourceID, std::shared_ptr<EntityTreeNode> parentEntityTreeNode);
    std::shared_ptr<EntityTreeNode> FindNode(const std::shared_ptr<Resource> templateResource, const unsigned int entityIndex, std::shared_ptr<EntityTreeNode> entityTreeNode) const;
    void SearchEntityNameInTree(std::shared_ptr<EntityTreeNode> node, const std::string& entityName, std::unordered_map<std::shared_ptr<EntityTreeNode>, std::shared_ptr<EntityTreeNode>>& parentMap);
    void SearchTypeNameInTree(std::shared_ptr<EntityTreeNode> node, const std::string& typeName, std::unordered_map<std::shared_ptr<EntityTreeNode>, std::shared_ptr<EntityTreeNode>>& parentMap);
    std::shared_ptr<EntityTreeNode> GeneratedFilteredEntityTree(const std::unordered_map<std::shared_ptr<EntityTreeNode>, std::shared_ptr<EntityTreeNode>>& parentMap);

    static const std::string& GetTEMPReferenceType(std::shared_ptr<TemplateEntity> tempResource, const unsigned int referenceIndex);
    static const std::string& GetTEMPReferenceResourceID(std::shared_ptr<TemplateEntity> tempResource, const unsigned int referenceIndex);
    static const std::string& GetTBLUReferenceResourceID(std::shared_ptr<TemplateEntityBlueprint> tbluResource, const unsigned int referenceIndex);
    static void LoadAndDeserializeTEMP(std::shared_ptr<TemplateEntity> tempResource);
    static void LoadAndDeserializeTBLU(std::shared_ptr<TemplateEntityBlueprint> tbluResource);

    std::shared_ptr<TemplateEntity> tempResource;
    bool isEntityTreeGenerated;
    std::shared_ptr<EntityTreeNode> rootNode;
    std::shared_ptr<EntityTreeNode> selectedEntityTreeNode;
    std::shared_ptr<EntityTreeNode> filteredTreeRootNode;
    bool scrollToEntity;
    SelectedEntityCallback selectedEntityCallback;
    char entityName[256] = { "" };
    bool showNewEntityModal;
    bool showRenameEntityModal;
};
