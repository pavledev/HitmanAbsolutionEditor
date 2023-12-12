#include <IconsMaterialDesignIcons.h>

#include "UI/Panels/SceneHierarchyPanel2.h"
#include "Utility/StringUtility.h"
#include "Rendering/Scene/Model.h"
#include "Rendering/Scene/PointLight.h"
#include "Rendering/Scene/Camera.h"
#include "Rendering/Scene/Grid.h"
#include "Rendering/Scene/SceneRenderer.h"
#include "Rendering/Scene/Skeleton.h"

SceneHierarchyPanel2::SceneHierarchyPanel2(const char* name, const char* icon, std::shared_ptr<RenderPrimitive> renderPrimitive) : BasePanel(name, icon)
{
    this->renderPrimitive = renderPrimitive;
    rootEntity = nullptr;
    hoveredEntity = nullptr;
    selectedEntity = nullptr;
}

std::shared_ptr<Entity> SceneHierarchyPanel2::GetRootEntity()
{
    return rootEntity;
}

void SceneHierarchyPanel2::SetSelectedEntity(std::shared_ptr<Entity> entity)
{
    selectedEntity = entity;
}

void SceneHierarchyPanel2::CreateEntities()
{
    std::string rootEntityName = std::format("{} Scene", ICON_MDI_VIEW_LIST);
    std::string modelEntityName = std::format("{} {}", ICON_MDI_SHAPE, renderPrimitive->GetName());
    std::string pointLightEntityName = std::format("{} Point Light", ICON_MDI_LIGHTBULB, renderPrimitive->GetName());
    std::string cameraEntityName = std::format("{} Camera", ICON_MDI_CAMERA, renderPrimitive->GetName());
    std::string gridEntityName = std::format("{} Grid", ICON_MDI_GRID, renderPrimitive->GetName());
    std::string skeletonEntityName = std::format("{} Skeleton", ICON_MDI_BONE, renderPrimitive->GetName());
    std::string pointLightMeshEntityName = std::format("{} Mesh", ICON_MDI_SHAPE, renderPrimitive->GetName());
    std::string collisionEntityName = std::format("{} Collision", ICON_MDI_SHAPE, renderPrimitive->GetName());

    rootEntity = std::make_shared<Entity>(rootEntityName);
    std::shared_ptr<Entity> modelEntity = std::make_shared<Entity>(modelEntityName);
    std::shared_ptr<Entity> pointLightEntity = std::make_shared<Entity>(pointLightEntityName);
    std::shared_ptr<Entity> cameraEntity = std::make_shared<Entity>(cameraEntityName);
    std::shared_ptr<Entity> gridEntity = std::make_shared<Entity>(gridEntityName);

    rootEntity.get()->Initialize();
    modelEntity.get()->Initialize();
    pointLightEntity.get()->Initialize();
    cameraEntity.get()->Initialize();
    gridEntity.get()->Initialize();

    modelEntity->AddComponent<Model>("Model", ICON_MDI_SHAPE);
    pointLightEntity->AddComponent<PointLight>("Point Light", ICON_MDI_LIGHTBULB);
    cameraEntity->AddComponent<Camera>("Camera", ICON_MDI_CAMERA);
    gridEntity->AddComponent<Grid>("Grid", ICON_MDI_GRID);

    rootEntity->AddChild(gridEntity);
    rootEntity->AddChild(modelEntity);
    rootEntity->AddChild(pointLightEntity);
    rootEntity->AddChild(cameraEntity);

    std::shared_ptr<Entity> pointLightMeshEntity = std::make_shared<Entity>(pointLightMeshEntityName);

    pointLightMeshEntity->Initialize();
    pointLightMeshEntity->AddComponent<Mesh>("Mesh", ICON_MDI_SHAPE);
    pointLightEntity->GetComponent<PointLight>()->SetMesh(pointLightMeshEntity->GetComponent<Mesh>());
    pointLightEntity->AddChild(pointLightMeshEntity);

    const std::vector<RenderPrimitive::Mesh*>& meshes = renderPrimitive->GetMeshes();

    for (size_t i = 0; i < meshes.size(); ++i)
    {
        std::string meshName = std::format("{} Mesh {}", ICON_MDI_SHAPE, i + 1);
        std::shared_ptr<Entity> meshEntity = std::make_shared<Entity>(meshName);

        meshEntity->Initialize();
        meshEntity->AddComponent<Mesh>("Mesh", ICON_MDI_SHAPE);
        modelEntity->GetComponent<Model>()->AddMesh(meshEntity->GetComponent<Mesh>());
        modelEntity->AddChild(meshEntity);
    }

    const BoneRig* boneRig = renderPrimitive->GetBoneRig();

    if (boneRig)
    {
        std::shared_ptr<Entity> skeletonEntity = std::make_shared<Entity>(skeletonEntityName);
        const std::vector<SBoneDefinition>& boneDefinitions = boneRig->GetBoneDefinitions();

        skeletonEntity->Initialize();
        skeletonEntity->AddComponent<Skeleton>("Skeleton", ICON_MDI_BONE);
        modelEntity->GetComponent<Model>()->SetSkeleton(skeletonEntity->GetComponent<Skeleton>());

        for (size_t i = 0; i < boneDefinitions.size(); ++i)
        {
            std::string boneName = std::format("{} {}", ICON_MDI_BONE, boneDefinitions[i].Name);
            std::shared_ptr<Entity> modelBoneEntity = std::make_shared<Entity>(boneName);

            modelBoneEntity->Initialize();
            modelBoneEntity->AddComponent<ModelBone>("Model Bone", ICON_MDI_BONE);

            for (unsigned int j = 0; j < 3; ++j)
            {
                std::string meshName = std::format("{} Mesh {}", ICON_MDI_SHAPE, j + 1);
                std::shared_ptr<Entity> meshEntity = std::make_shared<Entity>(meshName);

                meshEntity->Initialize();
                meshEntity->AddComponent<Mesh>("Mesh", ICON_MDI_SHAPE);
                modelBoneEntity->GetComponent<ModelBone>()->AddMesh(meshEntity->GetComponent<Mesh>());
                modelBoneEntity->AddChild(meshEntity);
            }

            skeletonEntity->GetComponent<Skeleton>()->AddModelBone(modelBoneEntity->GetComponent<ModelBone>());
            skeletonEntity->AddChild(modelBoneEntity);
        }

        modelEntity->AddChild(skeletonEntity);
    }

    const Physics* physics = renderPrimitive->GetPhysics();

    if (physics)
    {
        std::shared_ptr<Entity> collisionEntity = std::make_shared<Entity>(collisionEntityName);
        const std::vector<G2NxShapeDesc*>& shapeDescriptors = physics->GetCollisionShape().GetShapeDescriptors();

        collisionEntity->Initialize();
        collisionEntity->AddComponent<Collision>("Collision", ICON_MDI_SHAPE);
        modelEntity->GetComponent<Model>()->SetCollision(collisionEntity->GetComponent<Collision>());

        for (size_t i = 0; i < shapeDescriptors.size(); ++i)
        {
            std::string shapeType = G2NxShapeDesc::ConvertShapeTypeToString(shapeDescriptors[i]->m_pDesc->getType());
            std::string meshName = std::format("{} {} Shape {}", ICON_MDI_SHAPE, shapeType, i + 1);
            std::shared_ptr<Entity> meshEntity = std::make_shared<Entity>(meshName);

            meshEntity->Initialize();
            meshEntity->AddComponent<Mesh>("Mesh", ICON_MDI_SHAPE);
            collisionEntity->GetComponent<Collision>()->AddMesh(meshEntity->GetComponent<Mesh>());
            collisionEntity->AddChild(meshEntity);
        }

        modelEntity->AddChild(collisionEntity);
    }
}

void SceneHierarchyPanel2::Render()
{
    if (!Begin())
    {
        return;
    }

    if (renderPrimitive->GetMeshes().size() == 0)
    {
        End();

        return;
    }

    if (!rootEntity)
    {
        CreateEntities();
    }

    static ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY;

    if (ImGui::BeginTable("MeshesTable", 2, tableFlags))
    {
        ImGui::TableSetupColumn("   Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("   Visibility", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        RenderTree(rootEntity);

        ImGui::EndTable();
    }

    End();
}

void SceneHierarchyPanel2::RenderTree(std::shared_ptr<Entity> parentEntity)
{
    ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_SpanFullWidth;
    const bool isNodeHovered = hoveredEntity == parentEntity;
    const bool isNodeSelected = selectedEntity == parentEntity;
    static const ImVec4 headerSelectedColor = ImVec4(0.26f, 0.59f, 0.98f, 1.f);

    ImGui::PushID(parentEntity.get());

    ImGui::TableNextRow();

    if (parentEntity->HasChildren())
    {
        ImGui::TableSetColumnIndex(0);

        treeNodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;

        if (isNodeHovered && hoveredEntity != selectedEntity)
        {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(ImGuiCol_HeaderHovered));
        }
        else if (isNodeSelected)
        {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(headerSelectedColor));
            ImGui::PushStyleColor(ImGuiCol_Header, headerSelectedColor);
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, headerSelectedColor);
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, headerSelectedColor);
        }

        bool isParentNodeOpen = ImGui::TreeNodeEx("entity", treeNodeFlags, parentEntity->GetName().c_str());

        if (isNodeSelected)
        {
            ImGui::PopStyleColor(3);
        }

        if (ImGui::IsItemHovered() && !ImGui::IsItemToggledOpen())
        {
            hoveredEntity = parentEntity;
        }
        else if (hoveredEntity == parentEntity)
        {
            hoveredEntity = nullptr;
        }

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            selectedEntity = parentEntity;

            SceneRenderer::GetCamera()->SetSelectedEntity(selectedEntity);
        }

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("       %s", reinterpret_cast<const char*>(parentEntity->GetIsVisible() ? ICON_MDI_EYE_OUTLINE : ICON_MDI_EYE_OFF_OUTLINE));

        if (ImGui::IsItemClicked())
        {
            parentEntity->SetIsVisible(!parentEntity->GetIsVisible());
        }

        if (isParentNodeOpen)
        {
            const std::vector<std::shared_ptr<Entity>>& children = parentEntity->GetChildren();

            for (size_t i = 0; i < children.size(); ++i)
            {
                RenderTree(children[i]);
            }

            ImGui::TreePop();
        }
    }
    else
    {
        treeNodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        ImGui::TableSetColumnIndex(0);

        if (isNodeHovered && hoveredEntity != selectedEntity)
        {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(ImGuiCol_HeaderHovered));
        }
        else if (isNodeSelected)
        {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(headerSelectedColor));
            ImGui::PushStyleColor(ImGuiCol_Header, headerSelectedColor);
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, headerSelectedColor);
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, headerSelectedColor);
        }

        ImGui::TreeNodeEx("entity", treeNodeFlags, parentEntity->GetName().c_str());

        if (isNodeSelected)
        {
            ImGui::PopStyleColor(3);
        }

        if (ImGui::IsItemHovered() && !ImGui::IsItemToggledOpen())
        {
            hoveredEntity = parentEntity;
        }
        else if (hoveredEntity == parentEntity)
        {
            hoveredEntity = nullptr;
        }

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            selectedEntity = parentEntity;

            SceneRenderer::GetCamera()->SetSelectedEntity(selectedEntity);
        }

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("       %s", reinterpret_cast<const char*>(parentEntity->GetIsVisible() ? ICON_MDI_EYE_OUTLINE : ICON_MDI_EYE_OFF_OUTLINE));

        if (ImGui::IsItemClicked())
        {
            parentEntity->SetIsVisible(!parentEntity->GetIsVisible());
        }
    }

    ImGui::PopID();
}
