#include "Glacier/BoneRig/EBoneConstrainType.h"

#include "Resources/BoneRig.h"

void BoneRig::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());

	DeserializePrimaryHeader(binaryReader);
	DeserializeBoneDefinitions(binaryReader);
	DeserializeBindPoses(binaryReader);
	DeserializeBindPoseInverseGlobalMatrices(binaryReader);
	DeserializeBoneConstraints(binaryReader);
	DeserializePoseBones(binaryReader);
	DeserializePoseBoneIndices(binaryReader);
	DeserializePoseEntryIndices(binaryReader);
	DeserializePoseBoneCounts(binaryReader);
	DeserializeNames(binaryReader);
	DeserializeNamesEntryIndices(binaryReader);
	DeserializeFaceBoneIndices(binaryReader);
}

void BoneRig::DeserializePrimaryHeader(BinaryReader& binaryReader)
{
	unsigned int primaryHeaderOffset = binaryReader.Read<unsigned int>();

	binaryReader.Seek(primaryHeaderOffset, SeekOrigin::Begin);

	boneRig = binaryReader.Read<SBoneRig>();
}

void BoneRig::DeserializeBoneDefinitions(BinaryReader& binaryReader)
{
	binaryReader.Seek(boneRig.lBoneDefinitions, SeekOrigin::Begin);
	boneDefinitions.reserve(boneRig.m_NumberOfBones);

	for (unsigned int i = 0; i < boneRig.m_NumberOfBones; ++i)
	{
		boneDefinitions.push_back(binaryReader.Read<SBoneDefinition>());
	}
}

void BoneRig::DeserializeBindPoses(BinaryReader& binaryReader)
{
	binaryReader.Seek(boneRig.BindPose, SeekOrigin::Begin);
	bindPoses.reserve(boneRig.m_NumberOfBones);

	for (unsigned int i = 0; i < boneRig.m_NumberOfBones; ++i)
	{
		bindPoses.push_back(binaryReader.Read<SQV>());
	}
}

void BoneRig::DeserializeBindPoseInverseGlobalMatrices(BinaryReader& binaryReader)
{
	binaryReader.Seek(boneRig.BindPoseInvGlobalMats, SeekOrigin::Begin);
	bindPoseInverseGlobalMatrices.reserve(boneRig.m_NumberOfBones);

	for (unsigned int i = 0; i < boneRig.m_NumberOfBones; ++i)
	{
		const SMatrix43 bindPoseInverseGlobalMatrix = binaryReader.Read<SMatrix43>();
		SMatrix44 bindPoseInverseGlobalMatrix2;

		for (unsigned int j = 0; j < 4; ++j)
		{
			const float x = bindPoseInverseGlobalMatrix.r[j].x;
			const float y = bindPoseInverseGlobalMatrix.r[j].y;
			const float z = bindPoseInverseGlobalMatrix.r[j].z;
			const float w = j == 3 ? 1.f : 0.f;

			bindPoseInverseGlobalMatrix2.r[j] = SVector4(x, y, z, w);
		}

		bindPoseInverseGlobalMatrices.push_back(bindPoseInverseGlobalMatrix2);
	}
}

void BoneRig::DeserializeBoneConstraints(BinaryReader& binaryReader)
{
	binaryReader.Seek(boneRig.lBoneConstraintsHeader, SeekOrigin::Begin);

	boneConstraintsHeader = binaryReader.Read<SBoneConstraintsHeader>();

	boneConstraints.reserve(boneConstraintsHeader.m_lNrConstraints);

	const EBoneConstrainType type = static_cast<EBoneConstrainType>(binaryReader.Read<unsigned char>());

	binaryReader.Seek(-1, SeekOrigin::Current);

	if (type == EBoneConstrainType::BC_LOOKAT)
	{
		for (unsigned int i = 0; i < boneConstraintsHeader.m_lNrConstraints; ++i)
		{
			std::shared_ptr<SBoneConstraintLookAt> boneConstraintLookAt = std::make_shared<SBoneConstraintLookAt>();

			*boneConstraintLookAt = binaryReader.Read<SBoneConstraintLookAt>();

			boneConstraints.push_back(boneConstraintLookAt);
		}
	}
	else
	{
		for (unsigned int i = 0; i < boneConstraintsHeader.m_lNrConstraints; ++i)
		{
			std::shared_ptr<SBoneConstrainRotate> boneConstrainRotate = std::make_shared<SBoneConstrainRotate>();

			*boneConstrainRotate = binaryReader.Read<SBoneConstrainRotate>();

			boneConstraints.push_back(boneConstrainRotate);
		}
	}
}

void BoneRig::DeserializePoseBones(BinaryReader& binaryReader)
{
	binaryReader.Seek(boneRig.lPoseBoneHeader, SeekOrigin::Begin);
	poseBoneHeader = binaryReader.Read<SPoseBoneHeader>();

	binaryReader.Seek(poseBoneHeader.m_nPoseBoneArrayOffset, SeekOrigin::Begin);
	poseBones.reserve(poseBoneHeader.m_nPoseBoneCountTotal);

	for (int i = 0; i < poseBoneHeader.m_nPoseBoneCountTotal; ++i)
	{
		poseBones.push_back(binaryReader.Read<SPoseBone>());
	}
}

void BoneRig::DeserializePoseBoneIndices(BinaryReader& binaryReader)
{
	binaryReader.Seek(poseBoneHeader.m_nPoseBoneIndeArrayOffset, SeekOrigin::Begin);
	poseBoneIndices.reserve(poseBoneHeader.m_nPoseBoneCountTotal);

	for (int i = 0; i < poseBoneHeader.m_nPoseBoneCountTotal; ++i)
	{
		poseBoneIndices.push_back(binaryReader.Read<int>());
	}
}

void BoneRig::DeserializePoseEntryIndices(BinaryReader& binaryReader)
{
	binaryReader.Seek(poseBoneHeader.m_nPoseEntryIndexArrayOffset, SeekOrigin::Begin);
	poseEntryIndices.reserve(poseBoneHeader.m_nPoseCount);

	for (int i = 0; i < poseBoneHeader.m_nPoseCount; ++i)
	{
		poseEntryIndices.push_back(binaryReader.Read<int>());
	}
}

void BoneRig::DeserializePoseBoneCounts(BinaryReader& binaryReader)
{
	binaryReader.Seek(poseBoneHeader.m_nPoseBoneCountArrayOffset, SeekOrigin::Begin);
	poseBoneCounts.reserve(poseBoneHeader.m_nPoseCount);

	for (int i = 0; i < poseBoneHeader.m_nPoseCount; ++i)
	{
		poseBoneCounts.push_back(binaryReader.Read<int>());
	}
}

void BoneRig::DeserializeNames(BinaryReader& binaryReader)
{
	binaryReader.Seek(poseBoneHeader.m_nNamesListOffset, SeekOrigin::Begin);
	names.reserve(poseBoneHeader.m_nPoseCount);

	for (int i = 0; i < poseBoneHeader.m_nPoseCount; ++i)
	{
		names.push_back(binaryReader.ReadString());
	}
}

void BoneRig::DeserializeNamesEntryIndices(BinaryReader& binaryReader)
{
	binaryReader.Seek(poseBoneHeader.m_nNamesEntryIndexArrayOffset, SeekOrigin::Begin);
	namesEntryIndices.reserve(poseBoneHeader.m_nPoseCount);

	for (int i = 0; i < poseBoneHeader.m_nPoseCount; ++i)
	{
		namesEntryIndices.push_back(binaryReader.Read<int>());
	}
}

void BoneRig::DeserializeFaceBoneIndices(BinaryReader& binaryReader)
{
	binaryReader.Seek(poseBoneHeader.m_nFaceBoneIndexArrayOffset, SeekOrigin::Begin);
	faceBoneIndices.reserve(poseBoneHeader.m_nFaceBoneCount);

	for (int i = 0; i < poseBoneHeader.m_nFaceBoneCount; ++i)
	{
		faceBoneIndices.push_back(binaryReader.Read<int>());
	}
}

const std::vector<SBoneDefinition>& BoneRig::GetBoneDefinitions() const
{
	return boneDefinitions;
}

const std::vector<SQV>& BoneRig::GetBindPoses() const
{
	return bindPoses;
}

std::vector<SQV>& BoneRig::GetBindPoses()
{
	return bindPoses;
}

const std::vector<SMatrix44>& BoneRig::GetBindPoseInverseGlobalMatrices() const
{
	return bindPoseInverseGlobalMatrices;
}

std::vector<SMatrix44>& BoneRig::GetBindPoseInverseGlobalMatrices()
{
	return bindPoseInverseGlobalMatrices;
}

void BoneRig::SerializeToJson(const std::string& outputFilePath)
{
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

	writer.StartObject();

	writer.String("boneRig");
	boneRig.SerializeToJson(writer);

	writer.String("boneDefinitions");
	writer.StartArray();

	for (size_t i = 0; i < boneDefinitions.size(); ++i)
	{
		boneDefinitions[i].SerializeToJson(writer);
	}

	writer.EndArray();

	writer.String("bindPoses");
	writer.StartArray();

	for (size_t i = 0; i < bindPoses.size(); ++i)
	{
		bindPoses[i].SerializeToJson(writer);
	}

	writer.EndArray();

	writer.String("bindPoseInverseGlobalMatrices");
	writer.StartArray();

	for (size_t i = 0; i < bindPoseInverseGlobalMatrices.size(); ++i)
	{
		//bindPoseInverseGlobalMatrices[i].SerializeToJson(writer);
	}

	writer.EndArray();

	writer.String("boneConstraintsHeader");
	boneConstraintsHeader.SerializeToJson(writer);

	writer.String("boneConstraints");
	writer.StartArray();

	if (boneConstraintsHeader.m_lNrConstraints > 0)
	{
		EBoneConstrainType boneConstrainType = static_cast<EBoneConstrainType>(boneConstraints[0]->m_Type);

		if (boneConstrainType == EBoneConstrainType::BC_LOOKAT)
		{
			for (size_t i = 0; i < boneConstraints.size(); ++i)
			{
				std::static_pointer_cast<SBoneConstraintLookAt>(boneConstraints[i])->SerializeToJson(writer);
			}
		}
		else
		{
			for (size_t i = 0; i < boneConstraints.size(); ++i)
			{
				std::static_pointer_cast<SBoneConstrainRotate>(boneConstraints[i])->SerializeToJson(writer);
			}
		}
	}

	writer.EndArray();

	writer.String("poseBoneHeader");
	poseBoneHeader.SerializeToJson(writer);

	writer.String("poseBones");
	writer.StartArray();

	for (size_t i = 0; i < poseBones.size(); ++i)
	{
		poseBones[i].SerializeToJson(writer);
	}

	writer.EndArray();

	writer.String("poseBoneIndices");
	writer.StartArray();

	for (size_t i = 0; i < poseBoneIndices.size(); ++i)
	{
		writer.Int(poseBoneIndices[i]);
	}

	writer.EndArray();

	writer.String("poseEntryIndices");
	writer.StartArray();

	for (size_t i = 0; i < poseEntryIndices.size(); ++i)
	{
		writer.Int(poseEntryIndices[i]);
	}

	writer.EndArray();

	writer.String("poseBoneCounts");
	writer.StartArray();

	for (size_t i = 0; i < poseBoneCounts.size(); ++i)
	{
		writer.Int(poseBoneCounts[i]);
	}

	writer.EndArray();

	writer.String("names");
	writer.StartArray();

	for (size_t i = 0; i < names.size(); ++i)
	{
		writer.String(names[i].c_str());
	}

	writer.EndArray();

	writer.String("namesEntryIndices");
	writer.StartArray();

	for (size_t i = 0; i < namesEntryIndices.size(); ++i)
	{
		writer.Int(namesEntryIndices[i]);
	}

	writer.EndArray();

	writer.String("faceBoneIndices");
	writer.StartArray();

	for (size_t i = 0; i < faceBoneIndices.size(); ++i)
	{
		writer.Int(faceBoneIndices[i]);
	}

	writer.EndArray();

	writer.EndObject();

	std::ofstream outputFileStream = std::ofstream(outputFilePath);

	outputFileStream << stringBuffer.GetString();
}
