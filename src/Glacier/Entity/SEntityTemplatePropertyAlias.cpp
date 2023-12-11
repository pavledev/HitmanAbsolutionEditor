#include "Glacier/Entity/SEntityTemplatePropertyAlias.h"

void SEntityTemplatePropertyAlias::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("sAliasName");
    sAliasName.SerializeToJson(writer);

    writer.String("entityID");
    writer.Int(entityID);

    writer.String("sPropertyName");
    sPropertyName.SerializeToJson(writer);

    writer.EndObject();
}

void SEntityTemplatePropertyAlias::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
    unsigned int sAliasNameOffset = offset + offsetof(SEntityTemplatePropertyAlias, sAliasName);
    unsigned int entityIDOffset = offset + offsetof(SEntityTemplatePropertyAlias, entityID);
    unsigned int sPropertyNameOffset = offset + offsetof(SEntityTemplatePropertyAlias, sPropertyName);

    sAliasName.SerializeToMemory(binarySerializer, sAliasNameOffset);

    binarySerializer.WriteToMemory(&entityID, sizeof(int), entityIDOffset);

    sPropertyName.SerializeToMemory(binarySerializer, sPropertyNameOffset);
}

SEntityTemplatePropertyAlias* SEntityTemplatePropertyAlias::DeserializeFromJson(const rapidjson::Value& object)
{
    SEntityTemplatePropertyAlias* entityTemplatePropertyAlias = new SEntityTemplatePropertyAlias();

    entityTemplatePropertyAlias->sAliasName = *ZString::DeserializeFromJson(object["sAliasName"].GetString());
    entityTemplatePropertyAlias->entityID = object["entityID"].GetInt();
    entityTemplatePropertyAlias->sPropertyName = ZString(object["entityTemplates"].GetString());

    return entityTemplatePropertyAlias;
}
