#include "Glacier/Physics/SKinematicLinkedPrimResourceData.h"

void SKinematicLinkedPrimResourceData::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_pNxShapeList");
	writer.StartArray();

	for (size_t i = 0; i < m_aBonePrimitives.size(); ++i)
	{
		m_aBonePrimitives[i].SerializeToJson(writer);
	}

	writer.EndArray();

	writer.EndObject();
}
