#pragma once

#include "Component.h"
#include "Mesh.h"
#include "VirtualNode.h"
#include "EditBone.h"

class ModelBone : public Component
{
public:
	ModelBone(const char* name, const char* icon, std::weak_ptr<Entity> entity);
	void Initialize(const VirtualNode* virtualNode, EditBone& editBone);
	const float GetLength() const;
	static void SetLength(EditBone& editBone, const float length);
	static void AlignRoll(EditBone& editBone, const SVector3& no);
	static float CalculateRoll(EditBone& editBone, const SVector3& alignAxis, const bool axisOnly);
	void AddMesh(std::shared_ptr<Mesh> mesh);
	const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const;
	void Render() override;
	void RenderProperties() override;
	SMatrix44 GetBoneOffsetMatrix();
	void SetParent(std::weak_ptr<ModelBone> parent);
	SMatrix44 CalculateChannelMatrix();
	SMatrix44 ArmatureBoneMatrixToPoseMatrix(const SMatrix44& channelMatrix);
	void CalculateParentBoneTransform(SMatrix44& rotationScaleMatrix, SMatrix44& positionMatrix);
	void CalculateParentBoneTransformFromMatrices(const SMatrix44& boneOffsetMatrix, const SMatrix44* parentPoseMatrix,
		SMatrix44& rotationScaleMatrix, SMatrix44& positionMatrix);
	void CalculateArmatureBoneMatrix(const VirtualNode* virtualNode);
	static SMatrix33 ConvertArmatureBoneToMatrix(EditBone& editBone);

	/* -------------------------------------------------------------------- */
	/** \name Bone Vector, Roll Conversion
	 *
	 * Used for Objects and Pose Channels, since both can have multiple rotation representations.
	 *
	 * How it Works
	 * ============
	 *
	 * This is the bone transformation trick; they're hierarchical so each bone(b)
	 * is in the coord system of bone(b-1):
	 *
	 * arm_mat(b)= arm_mat(b-1) * yoffs(b-1) * d_root(b) * bone_mat(b)
	 *
	 * -> yoffs is just the y axis translation in parent's coord system
	 * -> d_root is the translation of the bone root, also in parent's coord system
	 *
	 * pose_mat(b)= pose_mat(b-1) * yoffs(b-1) * d_root(b) * bone_mat(b) * chan_mat(b)
	 *
	 * we then - in init deform - store the deform in chan_mat, such that:
	 *
	 * pose_mat(b)= arm_mat(b) * chan_mat(b)
	 *
	 * \{ */
	static SMatrix33 RollVectorToNormalizedMatrix(const SVector3& nor, const float roll);
	static SMatrix33 RollVectorToMatrix(const SVector3& vector, const float roll);

private:
	std::vector<std::shared_ptr<Mesh>> meshes;
	SVector3 armatureHeadPosition;
	SVector3 armatureTailPosition;
	SVector3 headPosition;
	SVector3 tailPosition;
	float length;
	SMatrix33 boneMatrix;
	SMatrix44 armatureMatrix;
	std::weak_ptr<ModelBone> parent;
	SVector3 poseBonePosition;
	SQuaternion poseBoneRotation;
	SVector3 poseBoneScale;
	SMatrix44 poseMatrix;

	/*
	* Roll along axis. We'll ultimately use the axis/angle method
	* for determining the transformation matrix of the bone. The axis
	* is tail-head while roll provides the angle.
	*/
	float armatureRoll;
	float roll;
};
