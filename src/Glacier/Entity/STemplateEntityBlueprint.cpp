#include "Glacier/Entity/STemplateEntityBlueprint.h"

void STemplateEntityBlueprint::SerializeToJson(const std::string& outputFilePath)
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

    writer.StartObject();

    writer.String("rootEntityIndex");
    writer.Int(rootEntityIndex);

    writer.String("entityTemplates");
    entityTemplates.SerializeToJson(writer);

    writer.String("pinConnections");
    pinConnections.SerializeToJson(writer);

    writer.String("inputPinForwardings");
    inputPinForwardings.SerializeToJson(writer);

    writer.String("outputPinForwardings");
    outputPinForwardings.SerializeToJson(writer);

    writer.EndObject();

    std::ofstream outputFileStream = std::ofstream(outputFilePath);

    outputFileStream << stringBuffer.GetString();

    outputFileStream.close();
}

void STemplateEntityBlueprint::SerializeToMemory(ZBinarySerializer& binarySerializer)
{
    unsigned int rootEntityIndexOffset = offsetof(STemplateEntityBlueprint, rootEntityIndex);
    unsigned int entityTemplatesOffset = offsetof(STemplateEntityBlueprint, entityTemplates);
    unsigned int pinConnectionsOffset = offsetof(STemplateEntityBlueprint, pinConnections);
    unsigned int inputPinForwardingsOffset = offsetof(STemplateEntityBlueprint, inputPinForwardings);
    unsigned int outputPinForwardingsOffset = offsetof(STemplateEntityBlueprint, outputPinForwardings);

    binarySerializer.WriteToMemory(&rootEntityIndex, sizeof(int), rootEntityIndexOffset);

    entityTemplates.SerializeToMemory(binarySerializer, entityTemplatesOffset);
    pinConnections.SerializeToMemory(binarySerializer, pinConnectionsOffset);
    inputPinForwardings.SerializeToMemory(binarySerializer, inputPinForwardingsOffset);
    outputPinForwardings.SerializeToMemory(binarySerializer, outputPinForwardingsOffset);
}

STemplateEntityBlueprint* STemplateEntityBlueprint::DeserializeFromJson(const rapidjson::Document& document)
{
    STemplateEntityBlueprint* templateEntityBlueprint = new STemplateEntityBlueprint();

    templateEntityBlueprint->rootEntityIndex = document["rootEntityIndex"].GetInt();
    templateEntityBlueprint->entityTemplates = *TArray<STemplateSubEntityBlueprint>::DeserializeFromJson(document["entityTemplates"].GetArray());
    templateEntityBlueprint->pinConnections = *TArray<SEntityTemplatePinConnection>::DeserializeFromJson(document["pinConnections"].GetArray());
    templateEntityBlueprint->inputPinForwardings = *TArray<SEntityTemplatePinConnection>::DeserializeFromJson(document["inputPinForwardings"].GetArray());
    templateEntityBlueprint->outputPinForwardings = *TArray<SEntityTemplatePinConnection>::DeserializeFromJson(document["outputPinForwardings"].GetArray());

    return templateEntityBlueprint;
}
