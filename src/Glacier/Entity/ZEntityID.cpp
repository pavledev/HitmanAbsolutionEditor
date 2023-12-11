#include "Glacier/Entity/ZEntityID.h"

void ZEntityID::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_sStr");
	m_sStr.SerializeToJson(writer);

	writer.EndObject();
}

void ZEntityID::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
	unsigned int strOffset = offset + offsetof(ZEntityID, m_sStr);

	m_sStr.SerializeToMemory(binarySerializer, strOffset);
}

ZEntityID* ZEntityID::DeserializeFromJson(const rapidjson::Value& object)
{
	ZEntityID* entityID = new ZEntityID();

	entityID->m_sStr = *ZString::DeserializeFromJson(object["m_sStr"].GetString());

	return entityID;
}
