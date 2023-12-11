#include "Glacier/Physics/SShatterConfig.h"

void SShatterConfig::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_aChildIndices");
	writer.StartArray();

	for (size_t i = 0; i < m_aBoneShards.size(); ++i)
	{
		m_aBoneShards[i].SerializeToJson(writer);
	}

	writer.EndArray();

	writer.EndObject();
}
