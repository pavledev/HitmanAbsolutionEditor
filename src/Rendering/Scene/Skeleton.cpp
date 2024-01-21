#include "Rendering/Scene/Skeleton.h"

Skeleton::Skeleton(const char* name, const char* icon, std::weak_ptr<Entity> entity) : Component(name, icon, entity)
{
}

void Skeleton::Initialize(std::shared_ptr<RenderPrimitive> renderPrimitive)
{
    std::shared_ptr<BoneRig> boneRig = renderPrimitive->GetBoneRig();
    const std::vector<SBoneDefinition>& boneDefinitions = boneRig->GetBoneDefinitions();
    std::vector<SQV>& bindPoses = boneRig->GetBindPoses();
    std::vector<VirtualNode> virtualNodes;

    InitializeVirtualNodes(0, virtualNodes, boneRig);

    for (size_t i = 1; i < bindPoses.size(); ++i)
    {
        float x = -bindPoses[i].m_Rotation.w128.m.m128_f32[0];
        float y = -bindPoses[i].m_Rotation.w128.m.m128_f32[1];
        float z = -bindPoses[i].m_Rotation.w128.m.m128_f32[2];
        float w = bindPoses[i].m_Rotation.w128.m.m128_f32[3];

        virtualNodes[i].bindPosition = SVector3(bindPoses[i].m_Translation);
        virtualNodes[i].bindRotation = SQuaternion(x, y, z, w);
    }

    virtualNodes[0].baseScale = SVector3(1.0f, 1.0f, 1.0f);

    //glTF Y-Up space --> Blender Z-up space
    //X, Y, Z-- > X, -Z, Y
    for (size_t i = 1; i < bindPoses.size(); ++i)
    {
        ConvertPosition(virtualNodes[i].bindPosition);
        ConvertRotation(virtualNodes[i].bindRotation);

        virtualNodes[i].basePosition = virtualNodes[i].bindPosition;
        virtualNodes[i].baseRotation = virtualNodes[i].bindRotation;
        virtualNodes[i].baseScale = SVector3(1.0f, 1.0f, 1.0f);
        virtualNodes[i].editBonePosition = virtualNodes[i].basePosition;
        virtualNodes[i].editBoneRotation = virtualNodes[i].baseRotation;
    }

    std::vector<SMatrix44>& bindPoseInverseGlobalMatrices = boneRig->GetBindPoseInverseGlobalMatrices();

    for (unsigned int i = 0; i < bindPoseInverseGlobalMatrices.size(); ++i)
    {
        ConvertMatrix(bindPoseInverseGlobalMatrices[i]);
    }

    PrettifyBones(&virtualNodes[0]);
    CalculateBoneMatrices(&virtualNodes[0]);

    std::vector<EditBone> editBones;

    editBones.resize(virtualNodes.size());

    for (size_t i = 0; i < virtualNodes.size(); ++i)
    {
        if (virtualNodes[i].parentIndex != -1)
        {
            const unsigned int parentIndex = virtualNodes[i].parentIndex;

            editBones[i].parent = &editBones[parentIndex];
            modelBones[i]->SetParent(modelBones[parentIndex]);
        }

        modelBones[i]->Initialize(&virtualNodes[i], editBones[i]);
        modelBones[i]->GetTransform()->SetParent(GetTransform());
        GetTransform()->AddChild(modelBones[i]->GetTransform());
    }

    Quaternion worldRotation = Quaternion::FromEulerAngles({ -90.f, 0.f, 0.f });

    GetTransform()->SetWorldRotation(worldRotation);
}

void Skeleton::InitializeVirtualNodes(const unsigned int parentIndex, std::vector<VirtualNode>& virtualNodes, std::shared_ptr<BoneRig> boneRig)
{
    const std::vector<SBoneDefinition>& boneDefinitions = boneRig->GetBoneDefinitions();

    virtualNodes.resize(boneDefinitions.size());

    for (size_t i = 0; i < boneDefinitions.size(); ++i)
    {
        if (boneDefinitions[i].lPrevBoneNr == parentIndex)
        {
            virtualNodes[i].parentIndex = parentIndex;
            virtualNodes[i].parent = &virtualNodes[parentIndex];
            virtualNodes[parentIndex].children.push_back(&virtualNodes[i]);

            InitializeVirtualNodes(i, virtualNodes, boneRig);
        }
    }
}

void Skeleton::AddModelBone(std::shared_ptr<ModelBone> modelBone)
{
    modelBones.push_back(modelBone);
}

void Skeleton::Render()
{
}

void Skeleton::RenderProperties()
{
}

void Skeleton::PrettifyBones(VirtualNode* virtualNode, const std::shared_ptr<SQuaternion> parentBoneRotation)
{
    virtualNode->boneLength = PickBoneLength(virtualNode);

    const std::shared_ptr<SQuaternion> rotation = PickBoneRotation(virtualNode, parentBoneRotation);

    if (rotation)
    {
        RotateBone(virtualNode, rotation);
    }

    for (size_t i = 0; i < virtualNode->children.size(); ++i)
    {
        PrettifyBones(virtualNode->children[i], rotation);
    }
}

float Skeleton::PickBoneLength(const VirtualNode* virtualNode)
{
    std::vector<SVector3> childBoneLocations;

    for (size_t i = 0; i < virtualNode->children.size(); ++i)
    {
        const float length = virtualNode->children[i]->editBonePosition.Length();

        if (length > minBoneLength)
        {
            childBoneLocations.push_back(virtualNode->children[i]->editBonePosition);
        }
    }

    if (childBoneLocations.size() > 0)
    {
        float minBoneLength2 = childBoneLocations[0].Length();

        for (size_t i = 0; i < childBoneLocations.size(); ++i)
        {
            const float length = childBoneLocations[i].Length();

            if (length < minBoneLength2)
            {
                minBoneLength2 = length;
            }
        }

        return minBoneLength2;
    }

    if (virtualNode->parent)
    {
        return virtualNode->parent->boneLength;
    }

    const float length = virtualNode->editBonePosition.Length();

    if (length > minBoneLength)
    {
        return length;
    }

    return 1.0f;
}

std::shared_ptr<SQuaternion> Skeleton::PickBoneRotation(const VirtualNode* virtualNode, const std::shared_ptr<SQuaternion> parentBoneRotation)
{
    //A bone's tip lies on its local +Y axis so rotating a bone let's us adjust the bone direction.

    if (boneHeuristic == BoneHeuristic::Blender)
    {
        return std::make_shared<SQuaternion>(std::powf(2, 0.5) / 2, std::powf(2, 0.5) / 2, 0, 0);
    }
    else
    {
        return Temperance(virtualNode, parentBoneRotation);
    }
}

std::shared_ptr<SQuaternion> Skeleton::Temperance(const VirtualNode* virtualNode, const std::shared_ptr<SQuaternion> parentBoneRotation)
{
    //Try to put our tip at the centroid of our children
    std::vector<SVector3> childBoneLocations;

    for (size_t i = 0; i < virtualNode->children.size(); ++i)
    {
        const float length = virtualNode->children[i]->editBonePosition.Length();

        if (length > minBoneLength)
        {
            childBoneLocations.push_back(virtualNode->children[i]->editBonePosition);
        }
    }

    if (childBoneLocations.size() > 0)
    {
        SVector3 centroid;

        for (size_t i = 0; i < childBoneLocations.size(); ++i)
        {
            centroid += childBoneLocations[i];
        }

        SQuaternion rotation = SQuaternion::RotationDifference(SVector3(0, 1, 0), centroid);

        if (boneHeuristic == BoneHeuristic::Temperance)
        {
            /*
            * Snap to the local axes; required for local_rotation to be
            * accurate when vnode has a non - uniform scaling.
            * FORTUNE skips this, so it may look better, but may have errors.
            */
            rotation = Math::GetNearbySignedPermutationMatrix(rotation).MatrixToQuaternion();
        }

        return std::make_shared<SQuaternion>(rotation);
    }

    return parentBoneRotation;
}

void Skeleton::RotateBone(VirtualNode* virtualNode, const std::shared_ptr<SQuaternion> rotation)
{
    //Rotate one edit bone without affecting anything else.

    SQuaternion rotation2 = virtualNode->editBoneRotation * *rotation;

    virtualNode->editBoneRotation = rotation2;

    //Cancel out the rotation so children aren't affected.
    SQuaternion invertedRotation = rotation->Conjugated();

    for (size_t i = 0; i < virtualNode->children.size(); ++i)
    {
        SVector3 position = invertedRotation * virtualNode->children[i]->editBonePosition;
        rotation2 = invertedRotation * virtualNode->children[i]->editBoneRotation;

        virtualNode->children[i]->editBonePosition = position;
        virtualNode->children[i]->editBoneRotation = rotation2;
    }

    //Need to rotate the bone's final TRS by the same amount so skinning isn't affected.
    LocalRotation(virtualNode, rotation);
}

void Skeleton::LocalRotation(VirtualNode* virtualNode, const std::shared_ptr<SQuaternion> rotation)
{
    /*
    * Appends a local rotation to vnode's world transform:
    * (new world transform) = (old world transform) @ (rot)
    * without changing the world transform of vnode's children.
    *
    * For correctness, rot must be a signed permutation of the axes
    * (eg. (X Y Z)->(X - Z Y)) OR vnode's scale must always be uniform.
    */

    SQuaternion rotationBefore2 = virtualNode->rotationBefore * *rotation;
    virtualNode->rotationBefore = rotationBefore2;

    //Append the inverse rotation after children's TRS to cancel it out.
    SQuaternion invertedRotation = rotation->Conjugated();

    for (size_t i = 0; i < virtualNode->children.size(); ++i)
    {
        virtualNode->children[i]->rotationAfter = invertedRotation * virtualNode->children[i]->rotationAfter;
    }
}

void Skeleton::CalculateBoneMatrices(VirtualNode* virtualNode)
{
    //Calculate the transformations from bone space to arma space in the bind pose and in the edit bone pose.
    SMatrix44 parentBindMatrix;
    SMatrix44 parentArmatureMatrix;

    if (virtualNode->parent)
    {
        parentBindMatrix = virtualNode->parent->bindArmatureMatrix;
        parentArmatureMatrix = virtualNode->parent->armatureMatrix;
    }
    else
    {
        parentBindMatrix = SMatrix44::Identity;
        parentArmatureMatrix = SMatrix44::Identity;
    }

    SMatrix44 localToParent = SMatrix44::CreateTranslation(virtualNode->bindPosition).Transposed() * SMatrix44(virtualNode->bindRotation.ToMatrix()).Transposed();
    virtualNode->bindArmatureMatrix = parentBindMatrix * localToParent;

    localToParent = SMatrix44::CreateTranslation(virtualNode->editBonePosition).Transposed() * SMatrix44(virtualNode->editBoneRotation.ToMatrix()).Transposed();
    virtualNode->armatureMatrix = parentArmatureMatrix * localToParent;

    for (size_t i = 0; i < virtualNode->children.size(); ++i)
    {
        CalculateBoneMatrices(virtualNode->children[i]);
    }
}

void Skeleton::ConvertPosition(SVector3& vector)
{
    const SVector3 tempTranslation = vector;

    vector.x = tempTranslation.x;
    vector.y = -tempTranslation.z;
    vector.z = tempTranslation.y;
}

void Skeleton::ConvertRotation(SQuaternion& quaternion)
{
    const SQuaternion tempRotation = quaternion;

    //Blender's quaternion components order: w, x, y, z
    /*quaternion.w128.m.m128_f32[0] = tempRotation.w128.m.m128_f32[3];
    quaternion.w128.m.m128_f32[1] = tempRotation.w128.m.m128_f32[0];
    quaternion.w128.m.m128_f32[2] = -tempRotation.w128.m.m128_f32[2];
    quaternion.w128.m.m128_f32[3] = tempRotation.w128.m.m128_f32[1];*/

    quaternion.w128.m.m128_f32[1] = -tempRotation.w128.m.m128_f32[2];
    quaternion.w128.m.m128_f32[2] = tempRotation.w128.m.m128_f32[1];
}

void Skeleton::ConvertMatrix(SMatrix44& matrix)
{
    const SMatrix44 temp = matrix;

    matrix.r[0] = { temp.v[0], -temp.v[8], temp.v[4], temp.v[12] };
    matrix.r[1] = { -temp.v[2], temp.v[10], -temp.v[6], -temp.v[14] };
    matrix.r[2] = { temp.v[1], -temp.v[9], temp.v[5], temp.v[13] };
    matrix.r[3] = { temp.v[3], -temp.v[11], temp.v[7], temp.v[15] };
}
