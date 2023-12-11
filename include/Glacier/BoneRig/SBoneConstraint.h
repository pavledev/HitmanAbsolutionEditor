#pragma once

#include <string>

#include "EBoneConstrainType.h"

struct SBoneConstraint
{
	static std::string ConvertTypeToString(const EBoneConstrainType boneConstrainType);

	unsigned char m_Type;
	unsigned char m_BoneIndex;
};
