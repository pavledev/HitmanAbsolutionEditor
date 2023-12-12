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
