#pragma once

#include <string>
#include <vector>

#include "Glacier/BoneRig/SBoneRig.h"
#include "Glacier/BoneRig/SBoneDefinition.h"
#include "Glacier/BoneRig/SBoneConstraintsHeader.h"
#include "Glacier/BoneRig/SBoneConstraintLookAt.h"
#include "Glacier/BoneRig/SBoneConstrainRotate.h"
#include "Glacier/BoneRig/SPoseBoneHeader.h"
#include "Glacier/BoneRig/SPoseBone.h"
#include "Glacier/Math/SQV.h"
#include "Glacier/Math/SMatrix43.h"
#include "Glacier/Math/SMatrix44.h"

#include "Resources/Resource.h"

class BoneRig : public Resource
{
public:
	void Deserialize();
	void DeserializePrimaryHeader(BinaryReader& binaryReader);
	void DeserializeBoneDefinitions(BinaryReader& binaryReader);
	void DeserializeBindPoses(BinaryReader& binaryReader);
	void DeserializeBindPoseInverseGlobalMatrices(BinaryReader& binaryReader);
	void DeserializeBoneConstraints(BinaryReader& binaryReader);
	void DeserializePoseBones(BinaryReader& binaryReader);
	void DeserializePoseBoneIndices(BinaryReader& binaryReader);
	void DeserializePoseEntryIndices(BinaryReader& binaryReader);
	void DeserializePoseBoneCounts(BinaryReader& binaryReader);
	void DeserializeNames(BinaryReader& binaryReader);
	void DeserializeNamesEntryIndices(BinaryReader& binaryReader);
	void DeserializeFaceBoneIndices(BinaryReader& binaryReader);
	const std::vector<SBoneDefinition>& GetBoneDefinitions() const;
	const std::vector<SQV>& GetBindPoses() const;
	std::vector<SQV>& GetBindPoses();
	const std::vector<SMatrix44>& GetBindPoseInverseGlobalMatrices() const;
	std::vector<SMatrix44>& GetBindPoseInverseGlobalMatrices();
	void SerializeToJson(const std::string& outputFilePath);

private:
	SBoneRig boneRig;
	std::vector<SBoneDefinition> boneDefinitions;
	std::vector<SQV> bindPoses;
	std::vector<SMatrix44> bindPoseInverseGlobalMatrices;
	SBoneConstraintsHeader boneConstraintsHeader;
	std::vector<std::shared_ptr<SBoneConstraint>> boneConstraints;
	SPoseBoneHeader poseBoneHeader;
	std::vector<SPoseBone> poseBones;
	std::vector<int> poseBoneIndices;
	std::vector<int> poseEntryIndices;
	std::vector<int> poseBoneCounts;
	std::vector<std::string> names;
	std::vector<int> namesEntryIndices;
	std::vector<int> faceBoneIndices;
};
