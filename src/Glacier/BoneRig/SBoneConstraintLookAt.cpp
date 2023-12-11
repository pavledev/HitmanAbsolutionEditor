#include "Glacier/BoneRig/SBoneConstraintLookAt.h"
#include "Glacier/Math/SVector3.h"

void SBoneConstraintLookAt::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_Type");
	writer.String(ConvertTypeToString(static_cast<EBoneConstrainType>(m_Type)).c_str());

	writer.String("m_BoneIndex");
	writer.Uint(m_BoneIndex);

	writer.String("m_lNrTargets");
	writer.Uint(m_lNrTargets);

	writer.String("m_lLookAtAxis");
	writer.Uint(m_lLookAtAxis);

	writer.String("m_lUpBoneAlignmentAxis");
	writer.Uint(m_lUpBoneAlignmentAxis);

	writer.String("m_lLookAtFlip");
	writer.Uint(m_lLookAtFlip);

	writer.String("m_lUpFlip");
	writer.Uint(m_lUpFlip);

	writer.String("m_UpnodeControl");
	writer.Uint(m_UpnodeControl);

	writer.String("m_UpNodeParentIdx");
	writer.Uint(m_UpNodeParentIdx);

	writer.String("m_TargetParentIdx");
	writer.StartArray();

	for (unsigned char i = 0; i < 2; ++i)
	{
		writer.Uint(m_TargetParentIdx[i]);
	}

	writer.EndArray();

	writer.String("m_lBoneTargetsWeights");
	writer.StartArray();

	for (unsigned char i = 0; i < 2; ++i)
	{
		writer.Double(m_lBoneTargetsWeights[i]);
	}

	writer.EndArray();

	SVector3 xAxis = SVector3(m_TargetPos[0][0], m_TargetPos[0][1], m_TargetPos[0][2]);
	SVector3 yAxis = SVector3(m_TargetPos[1][0], m_TargetPos[1][1], m_TargetPos[1][2]);

	writer.String("m_TargetPos");
	writer.StartObject();

	writer.String("xAxis");
	xAxis.SerializeToJson(writer);

	writer.String("yAxis");
	yAxis.SerializeToJson(writer);

	writer.EndObject();

	SVector3 upPos = SVector3(m_UpPos[0], m_UpPos[1], m_UpPos[2]);

	writer.String("m_UpPos");
	upPos.SerializeToJson(writer);

	writer.EndObject();
}
