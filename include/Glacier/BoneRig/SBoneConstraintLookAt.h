#pragma once

#include "rapidjson/prettywriter.h"

#include "SBoneConstraint.h"

struct SBoneConstraintLookAt : SBoneConstraint
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	unsigned char m_lNrTargets;
	unsigned char m_lLookAtAxis;
	unsigned char m_lUpBoneAlignmentAxis;
	unsigned char m_lLookAtFlip;
	unsigned char m_lUpFlip;
	unsigned char m_UpnodeControl;
	unsigned char m_UpNodeParentIdx;
	unsigned char m_TargetParentIdx[2];
	float m_lBoneTargetsWeights[2];
	float m_TargetPos[2][3];
	float m_UpPos[3];
};
