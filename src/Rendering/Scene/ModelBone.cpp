#include "Rendering/Scene/ModelBone.h"
#include "Rendering/Scene/Transform.h"
#include "Rendering/DirectXRenderer.h"
#include "Rendering/RHIStaticStates.h"
#include "Rendering/Scene/Geometry.h"
#include "Rendering/Renderer3D.h"

ModelBone::ModelBone(const char* name, const char* icon, std::weak_ptr<Entity> entity) : Component(name, icon, entity)
{
}

void ModelBone::Initialize(const VirtualNode* virtualNode, EditBone& editBone)
{
    //std::vector<VertexPositionNormal> boneVertices;
    static std::vector<VertexPosition> boneVertices;
    static std::vector<unsigned short> boneIndices;
    static std::vector<VertexPositionNormal> sphereVertices;
    static std::vector<unsigned short> sphereIndices;

    if (boneVertices.empty())
    {
        //Geometry::CreateOctahedralBone(boneVertices);
        Geometry::CreateOctahedralBone(boneVertices, boneIndices);
        Geometry::CreateSphere(sphereVertices, sphereIndices, 0.05f);
    }

    std::shared_ptr<Mesh> head = meshes[0];
    std::shared_ptr<Mesh> tail = meshes[1];
    std::shared_ptr<Mesh> bone = meshes[2];

    head->Initialize(sphereVertices, sphereIndices, Renderer3D::Shaders::SimpleVertex, Renderer3D::Shaders::SimplePixel, Vector3(0.721f, 0.709f, 0.709f));
    tail->Initialize(sphereVertices, sphereIndices, Renderer3D::Shaders::SimpleVertex, Renderer3D::Shaders::SimplePixel, Vector3(0.721f, 0.709f, 0.709f));
    //bone->Initialize(boneVertices, Renderer3D::Shaders::SimpleVertex, Renderer3D::Shaders::SimplePixel, Vector3(0.721f, 0.709f, 0.709f));
    bone->Initialize(boneVertices, boneIndices, Renderer3D::Shaders::SimpleVertex, Renderer3D::Shaders::SimplePixel, Vector3(0.721f, 0.709f, 0.709f));

    editBone.headPosition = virtualNode->armatureMatrix * SVector3(0, 0, 0);
    editBone.tailPosition = virtualNode->armatureMatrix * SVector3(0, 1, 0);

    SetLength(editBone, virtualNode->boneLength);
    AlignRoll(editBone, virtualNode->armatureMatrix * SVector3(0, 0, 1) - editBone.headPosition);

    armatureHeadPosition = editBone.headPosition;
    armatureTailPosition = editBone.tailPosition;
    armatureRoll = editBone.roll;
    roll = 0.f;

    if (virtualNode->parent)
    {
        ModelBone* parent = this->parent.lock().get();
        SMatrix44 inverseParentArmatureMatrix = parent->armatureMatrix.Inverted();

        headPosition = armatureHeadPosition - parent->armatureTailPosition;
        tailPosition = armatureTailPosition - parent->armatureTailPosition;

        headPosition = SMatrix33(inverseParentArmatureMatrix).Transposed() * headPosition;
        tailPosition = SMatrix33(inverseParentArmatureMatrix).Transposed() * tailPosition;
    }
    else
    {
        headPosition = armatureHeadPosition;
        tailPosition = armatureTailPosition;
    }

    //Set local matrix and arm_mat (restpose)
    CalculateArmatureBoneMatrix(virtualNode);

    // Get the ebone premat and its inverse.
    SMatrix33 preMat = ConvertArmatureBoneToMatrix(editBone);
    SMatrix33 inverseMat = preMat.Inverted();

    // Get the bone postmat.
    SMatrix33 postMat = SMatrix33(armatureMatrix);

    SMatrix33 difMat = postMat * inverseMat;

    roll = -atan2f(difMat[2][0], difMat[2][2]);

    // And set rest-position again.
    CalculateArmatureBoneMatrix(virtualNode);

    //BoneTRS = EditBone * PoseBone
    //Set PoseBone to make BoneTRS = vnode.trs.
    SVector3 position;
    SQuaternion rotation;
    SVector3 scale;

    virtualNode->GetTransform(position, rotation, scale);

    poseBonePosition = virtualNode->editBoneRotation.Conjugated() * (position - virtualNode->editBonePosition);
    poseBoneRotation = virtualNode->editBoneRotation.Conjugated() * rotation;
    poseBoneScale = scale;

    //pose_mat(b) = pose_mat(b-1) * yoffs(b-1) * d_root(b) * bone_mat(b) * chan_mat(b)
    SMatrix44 channelMatrix = CalculateChannelMatrix();
    poseMatrix = ArmatureBoneMatrixToPoseMatrix(channelMatrix);

    SVector3 headPosition;
    SQuaternion headRotation;
    SVector3 headScale;

    poseMatrix.Decompose(headPosition, headRotation, headScale);

    SMatrix44 poseMatrix2 = poseMatrix;
    SVector3 tailPosition;
    SQuaternion tailRotation;
    SVector3 tailScale;

    poseMatrix2.Rescale(SVector3(length, length, length));
    poseMatrix2.Translate(SVector3(0.0f, 1.0f, 0.0f));
    poseMatrix2.Decompose(tailPosition, tailRotation, tailScale);

    head->GetTransform()->SetLocalPosition(headPosition);
    bone->GetTransform()->SetLocalPosition(headPosition);
    tail->GetTransform()->SetLocalPosition(tailPosition);
    bone->GetTransform()->SetLocalRotation(headRotation);
    head->GetTransform()->SetLocalScale(length);
    bone->GetTransform()->SetLocalScale(length);
    tail->GetTransform()->SetLocalScale(length);

    meshes[0]->GetTransform()->SetParent(GetTransform());
    meshes[1]->GetTransform()->SetParent(GetTransform());
    meshes[2]->GetTransform()->SetParent(GetTransform());

    GetTransform()->AddChild(meshes[0]->GetTransform());
    GetTransform()->AddChild(meshes[1]->GetTransform());
    GetTransform()->AddChild(meshes[2]->GetTransform());
}

const float ModelBone::GetLength() const
{
    return length;
}

void ModelBone::SetLength(EditBone& editBone, const float length)
{
    editBone.length = length;

    SVector3 delta = editBone.tailPosition - editBone.headPosition;
    float length2 = 0;

    delta.Normalize(length2);

    if (length2 == 0.f)
    {
        //Zero length means directional information is lost. Choose arbitrary direction to avoid getting stuck.
        delta[2] = 1.0f;
    }

    editBone.tailPosition = editBone.headPosition + delta * length;
}

void ModelBone::AlignRoll(EditBone& editBone, const SVector3& no)
{
    editBone.roll = CalculateRoll(editBone, no, false);
}

float ModelBone::CalculateRoll(EditBone& editBone, const SVector3& alignAxis, const bool axisOnly)
{
    float roll = 0.f;

    SVector3 nor = editBone.tailPosition - editBone.headPosition;
    float length = 0;

    nor.Normalize(length);

    /* If tail == head or the bone is aligned with the axis... */
    if (length <= FLT_EPSILON || (fabsf(SVector3::Dot(alignAxis, nor)) >= (1.0f - FLT_EPSILON)))
    {
        return roll;
    }

    SMatrix33 mat = RollVectorToNormalizedMatrix(nor, 0.0f);

    /* project the new_up_axis along the normal */
    SVector3 vec = SVector3::ProjectNormalized(alignAxis, nor);
    SVector3 alignAxisProj = alignAxis - vec;

    if (axisOnly)
    {
        if (SVector3::Angle(alignAxisProj, mat[2]) > (float)(M_PI_2))
        {
            alignAxisProj.Negate();
        }
    }

    roll = SVector3::Angle(alignAxisProj, mat[2]);

    vec = SVector3::Cross(mat[2], alignAxisProj);

    if (SVector3::Dot(vec, nor) < 0.0f)
    {
        return -roll;
    }

    return roll;
}

void ModelBone::AddMesh(std::shared_ptr<Mesh> mesh)
{
    meshes.push_back(mesh);
}

const std::vector<std::shared_ptr<Mesh>>& ModelBone::GetMeshes() const
{
    return meshes;
}

void ModelBone::Render()
{
}

void ModelBone::RenderProperties()
{
}

SMatrix44 ModelBone::GetBoneOffsetMatrix()
{
    ModelBone* parent = this->parent.lock().get();

    //Bone transform itself.
    SMatrix44 boneOffsetMatrix = SMatrix44(boneMatrix);

    //The bone's root offset (is in the parent's coordinate system).
    boneOffsetMatrix[3][0] = headPosition.x;
    boneOffsetMatrix[3][1] = headPosition.y;
    boneOffsetMatrix[3][2] = headPosition.z;

    //Get the length translation of parent (length along y axis).
    boneOffsetMatrix[3][1] += parent->length;

    return boneOffsetMatrix;
}

void ModelBone::SetParent(std::weak_ptr<ModelBone> parent)
{
    this->parent = parent;
}

SMatrix44 ModelBone::CalculateChannelMatrix()
{
    SMatrix33 scaleMatrix = SMatrix33::ScaleToMatrix(poseBoneScale);

    /* quats are normalized before use to eliminate scaling issues */
    SQuaternion quaternion = poseBoneRotation.Normalized();
    SMatrix33 rotationMatrix = quaternion.ToMatrix();

    /* calculate matrix of bone (as 3x3 matrix, but then copy the 4x4) */
    SMatrix44 channelMatrix = scaleMatrix * rotationMatrix;

    channelMatrix[3][0] = poseBonePosition.x;
    channelMatrix[3][1] = poseBonePosition.y;
    channelMatrix[3][2] = poseBonePosition.z;

    return channelMatrix;
}

SMatrix44 ModelBone::ArmatureBoneMatrixToPoseMatrix(const SMatrix44& channelMatrix)
{
    SMatrix44 rotationScaleMatrix;
    SMatrix44 positionMatrix;
    SVector3 postScale = SVector3(1.f, 1.f, 1.f);

    CalculateParentBoneTransform(rotationScaleMatrix, positionMatrix);

    SVector3 position = SVector3(channelMatrix[3][0], channelMatrix[3][1], channelMatrix[3][2]);
    SVector3 newPosition = positionMatrix.Transposed() * position;
    SMatrix44 poseMatrix = channelMatrix * rotationScaleMatrix;

    poseMatrix[3][0] = newPosition.x;
    poseMatrix[3][1] = newPosition.y;
    poseMatrix[3][2] = newPosition.z;

    poseMatrix.Rescale(postScale);

    return poseMatrix;
}

void ModelBone::CalculateParentBoneTransform(SMatrix44& rotationScaleMatrix, SMatrix44& positionMatrix)
{
    ModelBone* parent = this->parent.lock().get();

    if (parent)
    {
        //yoffs(b-1) + root(b) + bonemat(b).
        SMatrix44 boneOffsetMatrix = GetBoneOffsetMatrix();

        CalculateParentBoneTransformFromMatrices(boneOffsetMatrix, &parent->poseMatrix, rotationScaleMatrix, positionMatrix);
    }
    else
    {
        CalculateParentBoneTransformFromMatrices(armatureMatrix, nullptr, rotationScaleMatrix, positionMatrix);
    }
}

void ModelBone::CalculateParentBoneTransformFromMatrices(const SMatrix44& boneOffsetMatrix, const SMatrix44* parentPoseMatrix,
    SMatrix44& rotationScaleMatrix, SMatrix44& positionMatrix)
{
    if (parentPoseMatrix)
    {
        rotationScaleMatrix = boneOffsetMatrix * *parentPoseMatrix;
        positionMatrix = rotationScaleMatrix;
    }
    else
    {
        rotationScaleMatrix = boneOffsetMatrix;
        positionMatrix = rotationScaleMatrix;
    }
}

void ModelBone::CalculateArmatureBoneMatrix(const VirtualNode* virtualNode)
{
    ModelBone* parent = this->parent.lock().get();

    //Bone Space
    const SVector3 vector = tailPosition - headPosition;

    length = vector.Length();

    boneMatrix = RollVectorToMatrix(vector, roll);

    if (virtualNode->parent)
    {
        //yoffs(b-1) + root(b) + bonemat(b)
        SMatrix44 boneOffsetMatrix = GetBoneOffsetMatrix();

        //Compose the matrix for this bone.
        armatureMatrix = boneOffsetMatrix * parent->armatureMatrix;
    }
    else
    {
        armatureMatrix = boneMatrix;
        armatureMatrix[3][0] = headPosition.x;
        armatureMatrix[3][1] = headPosition.y;
        armatureMatrix[3][2] = headPosition.z;
    }
}

SMatrix33 ModelBone::ConvertArmatureBoneToMatrix(EditBone& editBone)
{
    // Find the current bone matrix
    SVector3 delta = editBone.tailPosition - editBone.headPosition;
    float roll = editBone.roll;
    float length = 0;

    delta.Normalize(length);

    if (!length)
    {
        // Use the orientation of the parent bone if any.
        if (editBone.parent)
        {
            delta = editBone.parent->tailPosition - editBone.parent->headPosition;
            SVector3::Normalize(delta);
            roll = editBone.parent->roll;
        }
    }

    return RollVectorToNormalizedMatrix(delta, roll);
}

SMatrix33 ModelBone::RollVectorToNormalizedMatrix(const SVector3& nor, const float roll)
{
    /**
     * Given `v = (v.x, v.y, v.z)` our (normalized) bone vector, we want the rotation matrix M
     * from the Y axis (so that `M * (0, 1, 0) = v`).
     * - The rotation axis a lays on XZ plane, and it is orthonormal to v,
     *   hence to the projection of v onto XZ plane.
     * - `a = (v.z, 0, -v.x)`
     *
     * We know a is eigenvector of M (so M * a = a).
     * Finally, we have w, such that M * w = (0, 1, 0)
     * (i.e. the vector that will be aligned with Y axis once transformed).
     * We know w is symmetric to v by the Y axis.
     * - `w = (-v.x, v.y, -v.z)`
     *
     * Solving this, we get (x, y and z being the components of v):
     * <pre>
     *     | (x^2 * y + z^2) / (x^2 + z^2),   x,   x * z * (y - 1) / (x^2 + z^2) |
     * M = |  x * (y^2 - 1)  / (x^2 + z^2),   y,    z * (y^2 - 1)  / (x^2 + z^2) |
     *     | x * z * (y - 1) / (x^2 + z^2),   z,   (x^2 + z^2 * y) / (x^2 + z^2) |
     * </pre>
     *
     * This is stable as long as v (the bone) is not too much aligned with +/-Y
     * (i.e. x and z components are not too close to 0).
     *
     * Since v is normalized, we have `x^2 + y^2 + z^2 = 1`,
     * hence `x^2 + z^2 = 1 - y^2 = (1 - y)(1 + y)`.
     *
     * This allows to simplifies M like this:
     * <pre>
     *     | 1 - x^2 / (1 + y),   x,     -x * z / (1 + y) |
     * M = |                -x,   y,                   -z |
     *     |  -x * z / (1 + y),   z,    1 - z^2 / (1 + y) |
     * </pre>
     *
     * Written this way, we see the case v = +Y is no more a singularity.
     * The only one
     * remaining is the bone being aligned with -Y.
     *
     * Let's handle
     * the asymptotic behavior when bone vector is reaching the limit of y = -1.
     * Each of the four corner elements can vary from -1 to 1,
     * depending on the axis a chosen for doing the rotation.
     * And the "rotation" here is in fact established by mirroring XZ plane by that given axis,
     * then inversing the Y-axis.
     * For sufficiently small x and z, and with y approaching -1,
     * all elements but the four corner ones of M will degenerate.
     * So let's now focus on these corner elements.
     *
     * We rewrite M so that it only contains its four corner elements,
     * and combine the `1 / (1 + y)` factor:
     * <pre>
     *                    | 1 + y - x^2,        -x * z |
     * M* = 1 / (1 + y) * |                            |
     *                    |      -x * z,   1 + y - z^2 |
     * </pre>
     *
     * When y is close to -1, computing 1 / (1 + y) will cause severe numerical instability,
     * so we use a different approach based on x and z as inputs.
     * We know `y^2 = 1 - (x^2 + z^2)`, and `y < 0`, hence `y = -sqrt(1 - (x^2 + z^2))`.
     *
     * Since x and z are both close to 0, we apply the binomial expansion to the second order:
     * `y = -sqrt(1 - (x^2 + z^2)) = -1 + (x^2 + z^2) / 2 + (x^2 + z^2)^2 / 8`, which allows
     * eliminating the problematic `1` constant.
     *
     * A first order expansion allows simplifying to this, but second order is more precise:
     * <pre>
     *                        |  z^2 - x^2,  -2 * x * z |
     * M* = 1 / (x^2 + z^2) * |                         |
     *                        | -2 * x * z,   x^2 - z^2 |
     * </pre>
     *
     * P.S. In the end, this basically is a heavily optimized version of Damped Track +Y.
     */

    const float SAFE_THRESHOLD = 6.1e-3f;     /* Theta above this value has good enough precision. */
    const float CRITICAL_THRESHOLD = 2.5e-4f; /* True singularity if XZ distance is below this. */
    const float THRESHOLD_SQUARED = CRITICAL_THRESHOLD * CRITICAL_THRESHOLD;

    const float x = nor[0];
    const float y = nor[1];
    const float z = nor[2];

    float theta = 1.0f + y;                /* Remapping Y from [-1,+1] to [0,2]. */
    const float thetaAlt = x * x + z * z; /* Squared distance from origin in x,z plane. */
    SMatrix33 rMatrix;
    SMatrix33 bMatrix;

    /* Determine if the input is far enough from the true singularity of this type of
     * transformation at (0,-1,0), where roll becomes 0/0 undefined without a limit.
     *
     * When theta is close to zero (nor is aligned close to negative Y Axis),
     * we have to check we do have non-null X/Z components as well.
     * Also, due to float precision errors, nor can be (0.0, -0.99999994, 0.0) which results
     * in theta being close to zero. This will cause problems when theta is used as divisor.
     */
    if (theta > SAFE_THRESHOLD || thetaAlt > THRESHOLD_SQUARED)
    {
        /* nor is *not* aligned to negative Y-axis (0,-1,0). */

        bMatrix[0][1] = -x;
        bMatrix[1][0] = x;
        bMatrix[1][1] = y;
        bMatrix[1][2] = z;
        bMatrix[2][1] = -z;

        if (theta <= SAFE_THRESHOLD)
        {
            /* When nor is close to negative Y axis (0,-1,0) the theta precision is very bad,
             * so recompute it from x and z instead, using the series expansion for sqrt. */
            theta = thetaAlt * 0.5f + thetaAlt * thetaAlt * 0.125f;
        }

        bMatrix[0][0] = 1 - x * x / theta;
        bMatrix[2][2] = 1 - z * z / theta;
        bMatrix[2][0] = bMatrix[0][2] = -x * z / theta;
    }
    else
    {
        /* nor is very close to negative Y axis (0,-1,0): use simple symmetry by Z axis. */
        bMatrix.Unit();
        bMatrix[0][0] = bMatrix[1][1] = -1.0;
    }

    /* Make Roll matrix */
    rMatrix = SMatrix33::NormalizedAxisAngleToMatrix(nor, roll);

    /* Combine and output result */
    return bMatrix * rMatrix;
}

SMatrix33 ModelBone::RollVectorToMatrix(const SVector3& vector, const float roll)
{
    const SVector3 nor = SVector3::Normalize(vector);

    return RollVectorToNormalizedMatrix(nor, roll);
}
