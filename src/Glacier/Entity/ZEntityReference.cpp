#include "Glacier/Entity/ZEntityReference.h"

ZEntityReference::ZEntityReference(const ZEntityID& entityID, const ZString& exposedEntity)
{
	m_EntityID = entityID;
	m_sExposedEntity = exposedEntity;
}

void ZEntityReference::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_EntityID");
	m_EntityID.SerializeToJson(writer);

	writer.String("m_sExposedEntity");
	m_sExposedEntity.SerializeToJson(writer);

	writer.EndObject();
}

void ZEntityReference::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
	unsigned int entityIDOffset = offset + offsetof(ZEntityReference, m_EntityID);
	unsigned int exposedEntityOffset = offset + offsetof(ZEntityReference, m_sExposedEntity);

	m_EntityID.SerializeToMemory(binarySerializer, entityIDOffset);
	m_sExposedEntity.SerializeToMemory(binarySerializer, exposedEntityOffset);
}

ZEntityReference* ZEntityReference::DeserializeFromJson(const rapidjson::Value& object)
{
	ZEntityReference* entityReference = new ZEntityReference();

	entityReference->m_EntityID = *ZEntityID::DeserializeFromJson(object["entityIndex"]);
	entityReference->m_sExposedEntity = *ZString::DeserializeFromJson(object["exposedEntity"].GetString());

	return entityReference;
}
