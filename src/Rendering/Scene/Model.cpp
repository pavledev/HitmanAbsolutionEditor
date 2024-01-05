#include "Utility/UI.h"
#include "Rendering/Scene/Model.h"
#include "Rendering/PipelineState.h"
#include "Rendering/Scene/Transform.h"
#include "Rendering/DirectXRenderer.h"

Model::Model(const char* name, const char* icon, std::weak_ptr<Entity> entity) : Component(name, icon, entity)
{
    wireframe = false;
}

void Model::Initialize(std::shared_ptr<RenderPrimitive> renderPrimitive)
{
    const std::vector<std::shared_ptr<RenderPrimitive::Mesh>>& meshes = renderPrimitive->GetMeshes();

    for (size_t i = 0; i < meshes.size(); ++i)
    {
        std::shared_ptr<RenderPrimitive::Mesh> mesh = meshes[i];
        std::vector<std::shared_ptr<Resource>>& primReferences = renderPrimitive->GetReferences();

        const unsigned int matiReferenceIndex = mesh->GetMaterialID();
        std::shared_ptr<Resource> matiReference = primReferences[matiReferenceIndex];

        std::shared_ptr<Mesh> mesh2 = this->meshes[i];

        mesh2->Initialize(mesh, matiReference);
        mesh2->GetTransform()->SetParent(GetTransform());
        GetTransform()->AddChild(mesh2->GetTransform());

        lodMasks.insert(mesh->GetLODMask());
    }

    for (auto it = lodMasks.begin(); it != lodMasks.end(); ++it)
    {
        names.push_back(std::to_string(*it));
        flags.push_back(true);
    }

    const BoneRig* boneRig = renderPrimitive->GetBoneRig();

    if (boneRig)
    {
        skeleton->Initialize(renderPrimitive);
        skeleton->GetTransform()->SetParent(GetTransform());
        GetTransform()->AddChild(skeleton->GetTransform());
    }

    const Physics* physics = renderPrimitive->GetPhysics();

    if (physics)
    {
        collision->Initialize(renderPrimitive);
        collision->GetTransform()->SetParent(GetTransform());
        GetTransform()->AddChild(collision->GetTransform());
    }
}

const std::vector<std::shared_ptr<Mesh>>& Model::GetMeshes() const
{
    return meshes;
}

void Model::AddMesh(std::shared_ptr<Mesh> mesh)
{
    meshes.push_back(mesh);
}

void Model::SetSkeleton(std::shared_ptr<Skeleton> skeleton)
{
    this->skeleton = skeleton;
}

void Model::SetCollision(std::shared_ptr<Collision> collision)
{
    this->collision = collision;
}

void Model::Render()
{
}

void Model::RenderProperties()
{
    static constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed
        | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

    /*UI::BeginProperties();

    if (UI::Property("Wireframe", wireframe))
    {
        for (size_t i = 0; i < meshes.size(); ++i)
        {
            meshes[i]->SetWireframe(wireframe);
        }
    }

    UI::EndProperties();

    if (ImGui::TreeNodeEx("Level Of Detail", treeNodeFlags))
    {
        UI::BeginProperties();

        if (UI::Property("Display LODs", lod, 0u, 7u))
        {
            for (size_t i = 0; i < lodMasks.size(); ++i)
            {
                bool isLODInRange = false;

                for (unsigned int j = 0; j <= lod; ++j)
                {
                    unsigned int mask = (lod & (1 << j));

                    if (mask != 0)
                    {
                        isLODInRange = true;

                        break;
                    }
                }

                flags[i] = isLODInRange;
            }

            UpdateLODVisibility();
        }

        if (UI::Property("Mesh LODs", names, flags))
        {
            UpdateLODVisibility();
        }

        UI::EndProperties();

        ImGui::TreePop();
    }*/
}

void Model::UpdateLODVisibility()
{
    for (size_t i = 0; i < meshes.size(); ++i)
    {
        for (size_t j = 0; j < flags.size(); ++j)
        {
            unsigned int lodMask = *std::next(lodMasks.begin(), j);
            bool isLODInRange = flags[j];

            if (meshes[i]->GetLODMask() == lodMask)
            {
                meshes[i]->SetIsLODInRange(isLODInRange);
            }
        }
    }
}
