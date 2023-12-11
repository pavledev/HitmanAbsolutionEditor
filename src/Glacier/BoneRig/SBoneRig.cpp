#include "Glacier/BoneRig/SBoneRig.h"

void SBoneRig::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_NumberOfBones");
	writer.Uint(m_NumberOfBones);

	writer.String("m_NumberOfAnimatedBones");
	writer.Uint(m_NumberOfAnimatedBones);

	writer.String("lBoneDefinitions");
	writer.Uint(lBoneDefinitions);

	writer.String("BindPose");
	writer.Uint(BindPose);

	writer.String("BindPoseInvGlobalMats");
	writer.Uint(BindPoseInvGlobalMats);

	writer.String("lBoneConstraintsHeader");
	writer.Uint(lBoneConstraintsHeader);

	writer.String("lPoseBoneHeader");
	writer.Uint(lPoseBoneHeader);

	writer.String("lInvertGlobalBones");
	writer.Uint(lInvertGlobalBones);

	writer.String("pBoneMap");
	writer.Null();

	writer.EndObject();
}
