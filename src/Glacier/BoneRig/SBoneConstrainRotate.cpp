#include "Glacier/BoneRig/SBoneConstrainRotate.h"

void SBoneConstrainRotate::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_Type");
	writer.String(ConvertTypeToString(static_cast<EBoneConstrainType>(m_Type)).c_str());

	writer.String("m_BoneIndex");
	writer.Uint(m_BoneIndex);

	writer.String("m_ReferenceBoneIdx");
	writer.Uint(m_ReferenceBoneIdx);

	writer.String("m_TwistWeight");
	writer.Double(m_TwistWeight);

	writer.EndObject();
}
