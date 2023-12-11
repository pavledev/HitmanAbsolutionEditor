#pragma once

#include "rapidjson/prettywriter.h"

struct SBoneRig
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	unsigned int m_NumberOfBones;
	unsigned int m_NumberOfAnimatedBones;
	unsigned int lBoneDefinitions;
	unsigned int BindPose;
	unsigned int BindPoseInvGlobalMats;
	unsigned int lBoneConstraintsHeader;
	unsigned int lPoseBoneHeader;
	unsigned int lInvertGlobalBones;
	void* pBoneMap;
};
