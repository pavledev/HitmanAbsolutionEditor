#include "Glacier/Entity/ZEntityID.h"

ZEntityID::ZEntityID(const ZString& str)
{
	m_sStr = str;
}

void ZEntityID::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_sStr");
	m_sStr.SerializeToJson(writer);

	writer.EndObject();
}
