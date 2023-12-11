#include "Glacier/Entity/STemplateSubEntityBlueprint.h"

void STemplateSubEntityBlueprint::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("parentIndex");
    writer.Int(parentIndex);

    writer.String("entityTypeResourceIndex");
    writer.Int(entityTypeResourceIndex);

    writer.String("entityName");
    entityName.SerializeToJson(writer);

    writer.String("propertyAliases");
    propertyAliases.SerializeToJson(writer);

    writer.String("exposedEntities");
    exposedEntities.SerializeToJson(writer);

    writer.String("exposedInterfaces");
    exposedInterfaces.SerializeToJson(writer);

    writer.String("entitySubsets");
    entitySubsets.SerializeToJson(writer);

    writer.EndObject();
}

void STemplateSubEntityBlueprint::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, unsigned int entityIndex)
{
    writer.StartObject();

    writer.String("entityIndex");
    writer.Int(entityIndex);

    writer.String("parentIndex");
    writer.Int(parentIndex);

    writer.String("entityTypeResourceIndex");
    writer.Int(entityTypeResourceIndex);

    writer.String("entityName");
    entityName.SerializeToJson(writer);

    writer.String("propertyAliases");
    propertyAliases.SerializeToJson(writer);

    writer.String("exposedEntities");
    exposedEntities.SerializeToJson(writer);

    writer.String("exposedInterfaces");
    exposedInterfaces.SerializeToJson(writer);

    writer.String("entitySubsets");
    entitySubsets.SerializeToJson(writer);

    writer.EndObject();
}

void STemplateSubEntityBlueprint::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
    unsigned int parentIndexOffset = offset + offsetof(STemplateSubEntityBlueprint, parentIndex);
    unsigned int entityTypeResourceIndexOffset = offset + offsetof(STemplateSubEntityBlueprint, entityTypeResourceIndex);
    unsigned int entityNameOffset = offset + offsetof(STemplateSubEntityBlueprint, entityName);
    unsigned int propertyAliasesOffset = offset + offsetof(STemplateSubEntityBlueprint, propertyAliases);
    unsigned int exposedEntitiesOffset = offset + offsetof(STemplateSubEntityBlueprint, exposedEntities);
    unsigned int exposedInterfacesOffset = offset + offsetof(STemplateSubEntityBlueprint, exposedInterfaces);
    unsigned int entitySubsetsOffset = offset + offsetof(STemplateSubEntityBlueprint, entitySubsets);

    binarySerializer.WriteToMemory(&parentIndex, sizeof(int), parentIndexOffset);
    binarySerializer.WriteToMemory(&entityTypeResourceIndex, sizeof(int), entityTypeResourceIndexOffset);

    entityName.SerializeToMemory(binarySerializer, entityNameOffset);
    propertyAliases.SerializeToMemory(binarySerializer, propertyAliasesOffset);
    exposedEntities.SerializeToMemory(binarySerializer, exposedEntitiesOffset);
    exposedInterfaces.SerializeToMemory(binarySerializer, exposedInterfacesOffset);
    entitySubsets.SerializeToMemory(binarySerializer, entitySubsetsOffset);
}

STemplateSubEntityBlueprint* STemplateSubEntityBlueprint::DeserializeFromJson(const rapidjson::Value& object)
{
    STemplateSubEntityBlueprint* templateSubEntityBlueprint = new STemplateSubEntityBlueprint();

    templateSubEntityBlueprint->parentIndex = object["parentIndex"].GetInt();
    templateSubEntityBlueprint->entityTypeResourceIndex = object["entityTypeResourceIndex"].GetInt();
    templateSubEntityBlueprint->entityName = *ZString::DeserializeFromJson(object["entityName"].GetString());
    templateSubEntityBlueprint->propertyAliases = *TArray<SEntityTemplatePropertyAlias>::DeserializeFromJson(object["propertyAliases"].GetArray());
    templateSubEntityBlueprint->exposedEntities = *TArray<TPair<ZString, SEntityTemplateReference>>::DeserializeFromJson(object["exposedEntities"].GetArray());
    templateSubEntityBlueprint->exposedInterfaces = *TArray<TPair<ZString, int>>::DeserializeFromJson(object["exposedInterfaces"].GetArray());
    templateSubEntityBlueprint->entitySubsets = *TArray<TPair<ZString, SEntityTemplateEntitySubset>>::DeserializeFromJson(object["entitySubsets"].GetArray());

    return templateSubEntityBlueprint;
}
