#include <IconsMaterialDesignIcons.h>

#include "UI/Panels/SceneHierarchyPanel2.h"
#include "Utility/StringUtility.h"
#include "Rendering/Scene/Model.h"
#include "Rendering/Scene/DirectionalLight.h"
#include "Rendering/Scene/AmbientLight.h"
#include "Rendering/Scene/Camera.h"
#include "Rendering/Scene/Grid.h"
#include "Rendering/Scene/Skeleton.h"
#include "Rendering/Scene/BulletCollision.h"
#include "Rendering/Scene/CollisionBone.h"
#include "Resources/RenderPrimitive.h"
#include "Resources/Cloth.h"

SceneHierarchyPanel2::SceneHierarchyPanel2(const char* name, const char* icon, std::shared_ptr<Resource> resource) : BasePanel(name, icon)
{
    this->resource = resource;
    rootEntity = nullptr;
    hoveredEntity = nullptr;
    selectedEntity = nullptr;
    areEntitiesCreated = false;
}

void SceneHierarchyPanel2::OnSelectedEntityTreeNode(std::shared_ptr<Entity> selectedEntity)
{
    this->selectedEntity = selectedEntity;
}

void SceneHierarchyPanel2::CreateEntities()
{
    std::string rootEntityName = std::format("{} Scene", ICON_MDI_VIEW_LIST);
    std::string pointLightEntityName = std::format("{} Point Light", ICON_MDI_LIGHTBULB);
    std::string cameraEntityName = std::format("{} Camera", ICON_MDI_CAMERA);
    std::string gridEntityName = std::format("{} Grid", ICON_MDI_GRID);
    std::string directionalLightEntityName = std::format("{} Directional Light", ICON_MDI_SUN_WIRELESS);
    std::string ambientLightEntityName = std::format("{} Ambient Light", ICON_MDI_WALL_SCONCE_FLAT);

    rootEntity = std::make_shared<Entity>(rootEntityName);
    const std::vector<std::shared_ptr<Entity>>& children = rootEntity->GetChildren();
    std::shared_ptr<Entity> directionalLightEntity = std::make_shared<Entity>(directionalLightEntityName);
    std::shared_ptr<Entity> ambientLightEntity = std::make_shared<Entity>(ambientLightEntityName);
    std::shared_ptr<Entity> cameraEntity = std::make_shared<Entity>(cameraEntityName);
    std::shared_ptr<Entity> gridEntity = std::make_shared<Entity>(gridEntityName);

    rootEntity->Initialize();
    cameraEntity->Initialize();
    gridEntity->Initialize();

    directionalLightEntity->AddComponent<DirectionalLight>("Directional Light", ICON_MDI_LIGHTBULB);
    ambientLightEntity->AddComponent<AmbientLight>("Ambient Light", ICON_MDI_LIGHTBULB);

    cameraEntity->AddComponent<Camera>("Camera", ICON_MDI_CAMERA);
    cameraEntity->GetComponent<Camera>()->Initialize(75.f, 0.f, 0.1f, 3000.0f, Vector3(2.f, 0.5f, 0.f), Vector3(0.f, -90.f, 0.f), 0.f, -90.f);

    gridEntity->AddComponent<Grid>("Grid", ICON_MDI_GRID);
    gridEntity->GetComponent<Grid>()->Initialize();

    rootEntity->AddChild(gridEntity);
    rootEntity->AddChild(directionalLightEntity);
    rootEntity->AddChild(ambientLightEntity);
    rootEntity->AddChild(cameraEntity);

    directionalLightEntity->Initialize();
    ambientLightEntity->Initialize();

    switch (resource->GetResourceHeaderHeader().m_type)
    {
        case 'PRIM':
            CreateEntitiesForPRIM();
            break;
        case 'CLOS':
            CreateEntitiesForCLOS();
            break;
    }

    renderer3D->SetCamera(cameraEntity->GetComponent<Camera>());
    renderer3D->SetRootEntity(rootEntity);

    renderer3D->SetSelectedEntityCallback([this](std::shared_ptr<Entity> selectedEntity)
    {
        OnSelectedEntityTreeNode(selectedEntity);
    });

    areEntitiesCreated = true;
}

void SceneHierarchyPanel2::CreateEntitiesForPRIM()
{
    const std::vector<std::shared_ptr<Entity>>& children = rootEntity->GetChildren();
    std::string skeletonEntityName = std::format("{} Skeleton", ICON_MDI_BONE);
    std::string collisionEntityName = std::format("{} Collision", ICON_MDI_SHAPE);
    std::string bulletCollisionsEntityName = std::format("{} Bullet Collisions", ICON_MDI_SHAPE);

    std::shared_ptr<RenderPrimitive> renderPrimitive = std::static_pointer_cast<RenderPrimitive>(resource);

    std::string modelEntityName = std::format("{} {}", ICON_MDI_SHAPE, renderPrimitive->GetName());
    std::shared_ptr<Entity> modelEntity = std::make_shared<Entity>(modelEntityName);

    modelEntity->Initialize();
    modelEntity->AddComponent<Model>("Model", ICON_MDI_SHAPE);
    rootEntity->AddChild(modelEntity);

    const std::vector<std::shared_ptr<RenderPrimitive::Mesh>>& meshes = renderPrimitive->GetMeshes();

    for (size_t i = 0; i < meshes.size(); ++i)
    {
        std::string meshName = std::format("{} Mesh {}", ICON_MDI_SHAPE, i + 1);
        std::shared_ptr<Entity> meshEntity = std::make_shared<Entity>(meshName);

        meshEntity->Initialize();
        meshEntity->AddComponent<Mesh>("Mesh", ICON_MDI_SHAPE);
        meshEntity->GetComponent<Mesh>()->SetRenderer3D(renderer3D);
        modelEntity->GetComponent<Model>()->AddMesh(meshEntity->GetComponent<Mesh>());
        modelEntity->AddChild(meshEntity);
    }

    std::shared_ptr<BoneRig> boneRig = renderPrimitive->GetBoneRig();

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

            static const char* meshNames[3] = { "Head", "Tail", "Bone" };

            for (unsigned int j = 0; j < 3; ++j)
            {
                std::string meshName = std::format("{} {} Mesh", ICON_MDI_SHAPE, meshNames[j]);
                std::shared_ptr<Entity> meshEntity = std::make_shared<Entity>(meshName);

                meshEntity->Initialize();
                meshEntity->AddComponent<Mesh>("Mesh", ICON_MDI_SHAPE);
                meshEntity->GetComponent<Mesh>()->SetRenderer3D(renderer3D);
                modelBoneEntity->GetComponent<ModelBone>()->AddMesh(meshEntity->GetComponent<Mesh>());
                modelBoneEntity->AddChild(meshEntity);
            }

            skeletonEntity->GetComponent<Skeleton>()->AddModelBone(modelBoneEntity->GetComponent<ModelBone>());
            skeletonEntity->AddChild(modelBoneEntity);
        }

        modelEntity->AddChild(skeletonEntity);

        if (meshes[0]->GetSubType() != SPrimObject::SUBTYPE::SUBTYPE_WEIGHTED)
        {
            skeletonEntity->SetIsVisible(false);
        }
    }

    std::shared_ptr<Physics> physics = renderPrimitive->GetPhysics();

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
            meshEntity->GetComponent<Mesh>()->SetRenderer3D(renderer3D);
            collisionEntity->GetComponent<Collision>()->AddMesh(meshEntity->GetComponent<Mesh>());
            collisionEntity->AddChild(meshEntity);
        }

        modelEntity->AddChild(collisionEntity);

        collisionEntity->SetIsVisible(false);
    }

    if (renderPrimitive->HasBulletCollisionData())
    {
        std::shared_ptr<Entity> bulletCollisionsEntity = std::make_shared<Entity>(bulletCollisionsEntityName);
        const std::vector<std::shared_ptr<RenderPrimitive::Mesh>>& meshes = renderPrimitive->GetMeshes();

        bulletCollisionsEntity->Initialize();

        for (size_t i = 0; i < meshes.size(); ++i)
        {
            if (meshes[i]->GetSubMesh().lCollision == 0)
            {
                continue;
            }

            const SPrimObject::SUBTYPE subType = meshes[i]->GetSubType();

            if (subType == SPrimObject::SUBTYPE::SUBTYPE_STANDARD)
            {
                const std::shared_ptr<RenderPrimitive::StandardMesh> standardMesh = std::static_pointer_cast<RenderPrimitive::StandardMesh>(meshes[i]);
                const RenderPrimitive::CollisionBox& collisionBox = standardMesh->GetCollisionBox();
                const Vector3 boundingBoxMin = standardMesh->GetBoundingBoxMin();
                const Vector3 boundingBoxMax = standardMesh->GetBoundingBoxMax();

                std::string bulletCollisionEntityName = std::format("{} Bullet Collision (Mesh {})", ICON_MDI_SHAPE, i + 1);
                std::shared_ptr<Entity> bulletCollisionEntity = std::make_shared<Entity>(bulletCollisionEntityName);

                bulletCollisionEntity->Initialize();
                bulletCollisionEntity->AddComponent<BulletCollision>("Bullet Collision", ICON_MDI_SHAPE);
                bulletCollisionEntity->GetComponent<BulletCollision>()->Initialize(collisionBox, boundingBoxMin, boundingBoxMax, bulletCollisionEntity, renderer3D);

                bulletCollisionsEntity->AddChild(bulletCollisionEntity);
                bulletCollisionEntity->GetComponent<BulletCollision>()->GetTransform()->SetParent(bulletCollisionsEntity->GetComponent<Transform>());
                bulletCollisionsEntity->GetComponent<Transform>()->AddChild(bulletCollisionEntity->GetComponent<BulletCollision>()->GetTransform());
            }
            else if (subType == SPrimObject::SUBTYPE::SUBTYPE_LINKED)
            {
                const std::shared_ptr<RenderPrimitive::LinkedMesh> linkedMesh = std::static_pointer_cast<RenderPrimitive::LinkedMesh>(meshes[i]);
                const std::vector<RenderPrimitive::CollisionBox>& collisionBoxes = linkedMesh->GetCollisionBoxes();
                const Vector3 boundingBoxMin = linkedMesh->GetBoundingBoxMin();
                const Vector3 boundingBoxMax = linkedMesh->GetBoundingBoxMax();

                std::string collisionBoneEntityName = std::format("{} Collision Bone (Mesh {})", ICON_MDI_SHAPE, i + 1);
                std::shared_ptr<Entity> collisionBoneEntity = std::make_shared<Entity>(collisionBoneEntityName);

                collisionBoneEntity->Initialize();
                collisionBoneEntity->AddComponent<CollisionBone>("Collision Bone", ICON_MDI_SHAPE);
                collisionBoneEntity->GetComponent<CollisionBone>()->Initialize(meshes[i], collisionBoneEntity, renderer3D);

                bulletCollisionsEntity->AddChild(collisionBoneEntity);
                collisionBoneEntity->GetComponent<CollisionBone>()->GetTransform()->SetParent(bulletCollisionsEntity->GetComponent<Transform>());
                bulletCollisionsEntity->GetComponent<Transform>()->AddChild(collisionBoneEntity->GetComponent<CollisionBone>()->GetTransform());
            }
        }

        modelEntity->AddChild(bulletCollisionsEntity);
        bulletCollisionsEntity->GetComponent<Transform>()->SetParent(modelEntity->GetComponent<Model>()->GetTransform());
        modelEntity->GetComponent<Model>()->GetTransform()->AddChild(bulletCollisionsEntity->GetComponent<Transform>());

        bulletCollisionsEntity->SetIsVisible(false);
    }

    children[children.size() - 1]->GetComponent<Model>()->Initialize(renderPrimitive);
}

void SceneHierarchyPanel2::CreateEntitiesForBORG()
{
    const std::vector<std::shared_ptr<Entity>>& children = rootEntity->GetChildren();
    std::shared_ptr<BoneRig> boneRig = std::static_pointer_cast<BoneRig>(resource);

    std::string skeletonEntityName = std::format("{} Skeleton", ICON_MDI_BONE);
    std::shared_ptr<Entity> skeletonEntity = std::make_shared<Entity>(skeletonEntityName);
    const std::vector<SBoneDefinition>& boneDefinitions = boneRig->GetBoneDefinitions();

    skeletonEntity->Initialize();
    skeletonEntity->AddComponent<Skeleton>("Skeleton", ICON_MDI_BONE);

    for (size_t i = 0; i < boneDefinitions.size(); ++i)
    {
        std::string boneName = std::format("{} {}", ICON_MDI_BONE, boneDefinitions[i].Name);
        std::shared_ptr<Entity> modelBoneEntity = std::make_shared<Entity>(boneName);

        modelBoneEntity->Initialize();
        modelBoneEntity->AddComponent<ModelBone>("Model Bone", ICON_MDI_BONE);

        static const char* meshNames[3] = { "Head", "Tail", "Bone" };

        for (unsigned int j = 0; j < 3; ++j)
        {
            std::string meshName = std::format("{} {} Mesh", ICON_MDI_SHAPE, meshNames[j]);
            std::shared_ptr<Entity> meshEntity = std::make_shared<Entity>(meshName);

            meshEntity->Initialize();
            meshEntity->AddComponent<Mesh>("Mesh", ICON_MDI_SHAPE);
            meshEntity->GetComponent<Mesh>()->SetRenderer3D(renderer3D);
            modelBoneEntity->GetComponent<ModelBone>()->AddMesh(meshEntity->GetComponent<Mesh>());
            modelBoneEntity->AddChild(meshEntity);
        }

        skeletonEntity->GetComponent<Skeleton>()->AddModelBone(modelBoneEntity->GetComponent<ModelBone>());
        skeletonEntity->AddChild(modelBoneEntity);
    }

    rootEntity->AddChild(skeletonEntity);
    children[children.size() - 1]->GetComponent<Skeleton>()->Initialize(boneRig);

    Quaternion worldRotation = Quaternion::FromEulerAngles({ 0.f, 0.f, 180.f });

    children[children.size() - 1]->GetComponent<Skeleton>()->GetTransform()->SetWorldRotation(worldRotation);
}

void SceneHierarchyPanel2::CreateEntitiesForALOC()
{
    const std::vector<std::shared_ptr<Entity>>& children = rootEntity->GetChildren();
    std::shared_ptr<Physics> physics = std::static_pointer_cast<Physics>(resource);

    std::string collisionEntityName = std::format("{} Collision", ICON_MDI_SHAPE);
    std::shared_ptr<Entity> collisionEntity = std::make_shared<Entity>(collisionEntityName);
    const std::vector<G2NxShapeDesc*>& shapeDescriptors = physics->GetCollisionShape().GetShapeDescriptors();

    collisionEntity->Initialize();
    collisionEntity->AddComponent<Collision>("Collision", ICON_MDI_SHAPE);

    for (size_t i = 0; i < shapeDescriptors.size(); ++i)
    {
        std::string shapeType = G2NxShapeDesc::ConvertShapeTypeToString(shapeDescriptors[i]->m_pDesc->getType());
        std::string meshName = std::format("{} {} Shape {}", ICON_MDI_SHAPE, shapeType, i + 1);
        std::shared_ptr<Entity> meshEntity = std::make_shared<Entity>(meshName);

        meshEntity->Initialize();
        meshEntity->AddComponent<Mesh>("Mesh", ICON_MDI_SHAPE);
        meshEntity->GetComponent<Mesh>()->SetRenderer3D(renderer3D);
        collisionEntity->GetComponent<Collision>()->AddMesh(meshEntity->GetComponent<Mesh>());
        collisionEntity->AddChild(meshEntity);
    }

    rootEntity->AddChild(collisionEntity);
    children[children.size() - 1]->GetComponent<Collision>()->Initialize(physics);

    Quaternion worldRotation = Quaternion::FromEulerAngles({ -90.f, 0.f, 0.f });

    children[children.size() - 1]->GetComponent<Collision>()->GetTransform()->SetWorldRotation(worldRotation);
}

void SceneHierarchyPanel2::CreateEntitiesForCLOS()
{
    const std::vector<std::shared_ptr<Entity>>& children = rootEntity->GetChildren();
    std::shared_ptr<Cloth> cloth = std::static_pointer_cast<Cloth>(resource);

    std::string modelEntityName = std::format("{} {}", ICON_MDI_SHAPE, cloth->GetName());
    std::shared_ptr<Entity> modelEntity = std::make_shared<Entity>(modelEntityName);

    modelEntity->Initialize();
    modelEntity->AddComponent<Model>("Model", ICON_MDI_SHAPE);
    rootEntity->AddChild(modelEntity);

    std::string meshName = std::format("{} Mesh", ICON_MDI_SHAPE);
    std::shared_ptr<Entity> meshEntity = std::make_shared<Entity>(meshName);

    meshEntity->Initialize();
    meshEntity->AddComponent<Mesh>("Mesh", ICON_MDI_SHAPE);
    meshEntity->GetComponent<Mesh>()->SetRenderer3D(renderer3D);
    modelEntity->GetComponent<Model>()->AddMesh(meshEntity->GetComponent<Mesh>());
    modelEntity->AddChild(meshEntity);

    children[children.size() - 1]->GetComponent<Model>()->Initialize(cloth);
}

void SceneHierarchyPanel2::Render()
{
    if (!Begin())
    {
        return;
    }

    if (!resource->IsResourceDeserialized())
    {
        ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
        ImGui::Text("Loading model...");
        End();

        return;
    }

    if (!areEntitiesCreated)
    {
        ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
        ImGui::Text("Creating entities...");
        End();

        return;
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

            renderer3D->GetCamera()->SetSelectedEntity(selectedEntity);
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

            renderer3D->GetCamera()->SetSelectedEntity(selectedEntity);
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

void SceneHierarchyPanel2::OnResourceLoaded()
{
    CreateEntities();

    const std::vector<std::shared_ptr<Entity>>& children = rootEntity->GetChildren();

    for (size_t i = 0; i < children.size(); ++i)
    {
        if (children[i]->GetComponent<Camera>())
        {
            children[i]->GetComponent<Camera>()->SetRenderer3D(renderer3D);
        }
        else if (children[i]->GetComponent<Mesh>())
        {
            children[i]->GetComponent<Mesh>()->SetRenderer3D(renderer3D);
        }
        /*else if (children[i]->GetComponent<PointLight>())
        {
            children[i]->GetComponent<PointLight>()->SetRenderer3D(renderer3D);
        }*/
        else if (children[i]->GetComponent<DirectionalLight>())
        {
            children[i]->GetComponent<DirectionalLight>()->SetRenderer3D(renderer3D);
        }
        else if (children[i]->GetComponent<AmbientLight>())
        {
            children[i]->GetComponent<AmbientLight>()->SetRenderer3D(renderer3D);
        }
        else if (children[i]->GetComponent<Grid>())
        {
            children[i]->GetComponent<Grid>()->SetRenderer3D(renderer3D);
        }
    }
}

void SceneHierarchyPanel2::SetRenderer3D(std::shared_ptr<Renderer3D> renderer3D)
{
    this->renderer3D = renderer3D;
}
