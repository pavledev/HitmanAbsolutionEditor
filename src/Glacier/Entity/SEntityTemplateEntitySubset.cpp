#include "Glacier/Entity/SEntityTemplateEntitySubset.h"

void SEntityTemplateEntitySubset::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("subsetType");
    writer.String(subsetType->pTypeInfo->GetTypeName());

    writer.String("entities");
    entities.SerializeToJson(writer);

    writer.EndObject();
}

void SEntityTemplateEntitySubset::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
    unsigned int subsetTypeOffset = offset + offsetof(SEntityTemplateEntitySubset, subsetType);
    unsigned int entitiesOffset = offset + offsetof(SEntityTemplateEntitySubset, entities);

    subsetType->SerializeToMemory(binarySerializer, subsetTypeOffset);
    entities.SerializeToMemory(binarySerializer, entitiesOffset);
}

SEntityTemplateEntitySubset* SEntityTemplateEntitySubset::DeserializeFromJson(const rapidjson::Value& object)
{
    SEntityTemplateEntitySubset* entityTemplateEntitySubset = new SEntityTemplateEntitySubset();

    entityTemplateEntitySubset->subsetType = STypeID::DeserializeFromJson(object["subsetType"].GetString());
    entityTemplateEntitySubset->entities = *TArray<int>::DeserializeFromJson(object["entities"].GetArray());

    return entityTemplateEntitySubset;
}
