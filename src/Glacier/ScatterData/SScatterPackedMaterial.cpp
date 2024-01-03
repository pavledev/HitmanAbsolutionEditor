#include "Glacier/ScatterData/SScatterPackedMaterial.h"

void SScatterPackedMaterial::SerializeToJson(std::vector<SScatterPackedInstance>& instances, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_idResource");
	writer.String(ZRuntimeResourceID::QueryResourceID(m_idResource).c_str());

	writer.String("m_Base");
	m_Base.SerializeToJson(writer);

	writer.String("instances");
	writer.StartArray();

	for (size_t i = 0; i < instances.size(); ++i)
	{
		instances[i].SerializeToJson(writer);
	}

	writer.EndArray();

	writer.EndObject();
}
