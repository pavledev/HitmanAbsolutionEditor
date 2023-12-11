#include "Glacier/BoneRig/SBoneConstraintsHeader.h"

void SBoneConstraintsHeader::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_lNrConstraints");
	writer.Uint(m_lNrConstraints);

	writer.EndObject();
}
