#pragma once

#include "rapidjson/prettywriter.h"

#include "SBoneConstraint.h"

struct SBoneConstrainRotate : SBoneConstraint
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	unsigned char m_ReferenceBoneIdx;
	float m_TwistWeight;
};
