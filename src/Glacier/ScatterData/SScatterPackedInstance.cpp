#include "Glacier/ScatterData/SScatterPackedInstance.h"

void SScatterPackedInstance::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_nData");
	writer.StartArray();

	for (unsigned char i = 0; i < 11; ++i)
	{
		writer.Uint(m_nData[i]);
	}

	writer.EndArray();

	writer.EndObject();
}
