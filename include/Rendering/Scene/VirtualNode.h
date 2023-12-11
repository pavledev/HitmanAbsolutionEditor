#pragma once

#include <vector>

#include "Glacier/Math/SQuaternion.h"
#include "Glacier/Math/SMatrix44.h"
#include "Transform.h"

struct VirtualNode
{
	void GetTransform(SVector3& basePosition, SQuaternion& baseRotation, SVector3& baseScale) const;

	unsigned int parentIndex = -1;
	VirtualNode* parent = nullptr;
	std::vector<VirtualNode*> children;
	SVector3 basePosition;
	SQuaternion baseRotation;
	SVector3 baseScale;
	SVector3 editBonePosition;
	SQuaternion editBoneRotation;
	SVector3 bindPosition;
	SQuaternion bindRotation;
	float boneLength;
	SMatrix44 bindArmatureMatrix;
	SMatrix44 armatureMatrix;

	//Additional rotations before/after the base TRS.
	//Allows per-virtual node axis adjustment. See LocalRotation.
	Quaternion rotationAfter;
	Quaternion rotationBefore;
};
