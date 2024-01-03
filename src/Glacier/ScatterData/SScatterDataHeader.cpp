#include "Glacier/ScatterData/SScatterDataHeader.h"

void SScatterDataHeader::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_nType");
	writer.Uint(m_nType);

	writer.String("m_nMaterials");
	writer.Uint(m_nMaterials);

	writer.EndObject();
}
