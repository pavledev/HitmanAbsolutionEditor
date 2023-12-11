#include "Glacier/Physics/SKinematicBoneData.h"

void SKinematicBoneData::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_nGlobalBoneId");
	writer.Uint(m_nGlobalBoneId);

	writer.String("m_nParentBoneId");
	writer.Uint(m_nParentBoneId);

	writer.String("m_pShape");
	m_pShape.SerializeToJson(writer);

	writer.EndObject();
}
