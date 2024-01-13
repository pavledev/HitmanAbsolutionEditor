#pragma once

#include "Component.h"
#include "ModelBone.h"
#include "Resources/RenderPrimitive.h"
#include "Transform.h"
#include "VirtualNode.h"

class Skeleton : public Component
{
public:
	Skeleton(const char* name, const char* icon, std::weak_ptr<Entity> entity);
	void Initialize(std::shared_ptr<RenderPrimitive> renderPrimitive);
	void InitializeVirtualNodes(const unsigned int parentIndex, std::vector<VirtualNode>& virtualNodes, std::shared_ptr<BoneRig> boneRig);
	void AddModelBone(std::shared_ptr<ModelBone> modelBone);
	void Render() override;
	void RenderProperties() override;
	void PrettifyBones(VirtualNode* virtualNode, const SQuaternion* parentBoneRotation = nullptr);
	float PickBoneLength(const VirtualNode* virtualNode);
	SQuaternion PickBoneRotation(const VirtualNode* virtualNode, const SQuaternion* parentBoneRotation);
	SQuaternion Temperance(const VirtualNode* virtualNode, const SQuaternion* parentBoneRotation);
	void RotateBone(VirtualNode* virtualNode, const SQuaternion& rotation);
	void LocalRotation(VirtualNode* virtualNode, const SQuaternion& rotation);
	void CalculateBoneMatrices(VirtualNode* virtualNode);
	void ConvertPosition(SVector3& vector);
	void ConvertRotation(SQuaternion& SQuaternion);
	void ConvertMatrix(SMatrix44& matrix);

private:
	enum class BoneHeuristic
	{
		Blender, //(best for re-importing) - Good for re-importing glTFs exported from Blender. Bone tips are placed on their local +Y axis (in glTF space)
		Temperance, //(average) - Decent all-around strategy. A bone with one child has its tip placed on the local axis closest to its child
		Fortune //(may look better, less accurate) - Might look better than Temperance, but also might have errors. A bone with one child has its tip placed at its child's root. Non-uniform scalings may get messed up though, so beware
	};

	std::vector<std::shared_ptr<ModelBone>> modelBones;
	inline static const float minBoneLength = 0.004f;
	BoneHeuristic boneHeuristic = BoneHeuristic::Temperance;
};
