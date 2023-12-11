#include "Glacier/Entity/STemplateEntity.h"

std::string STemplateEntity::SerializeToJson()
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

    writer.StartObject();

    writer.String("blueprintIndexInResourceHeader");
    writer.Int(blueprintIndexInResourceHeader);

    writer.String("rootEntityIndex");
    writer.Int(rootEntityIndex);

    writer.String("entityTemplates");
    entityTemplates.SerializeToJson(writer);

    writer.EndObject();

    return stringBuffer.GetString();
}

void STemplateEntity::SerializeToMemory(ZBinarySerializer& binarySerializer)
{
    unsigned int blueprintIndexInResourceHeaderOffset = offsetof(STemplateEntity, blueprintIndexInResourceHeader);
    unsigned int rootEntityIndexOffset = offsetof(STemplateEntity, rootEntityIndex);
    unsigned int entityTemplatesOffset = offsetof(STemplateEntity, entityTemplates);

    binarySerializer.WriteToMemory(&blueprintIndexInResourceHeader, sizeof(int), blueprintIndexInResourceHeaderOffset);
    binarySerializer.WriteToMemory(&rootEntityIndex, sizeof(int), rootEntityIndexOffset);

    entityTemplates.SerializeToMemory(binarySerializer, entityTemplatesOffset);
}

STemplateEntity* STemplateEntity::DeserializeFromJson(const rapidjson::Document& document)
{
    STemplateEntity* templateEntity = new STemplateEntity();

    templateEntity->blueprintIndexInResourceHeader = document["blueprintIndexInResourceHeader"].GetInt();
    templateEntity->rootEntityIndex = document["rootEntityIndex"].GetInt();
    templateEntity->entityTemplates = *TArray<STemplateSubEntity>::DeserializeFromJson(document["entityTemplates"].GetArray());

    return templateEntity;
}
