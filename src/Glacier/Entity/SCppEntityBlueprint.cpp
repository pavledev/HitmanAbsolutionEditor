#include "Glacier/Entity/SCppEntityBlueprint.h"

void SCppEntityBlueprint::SerializeToJson(const std::string& outputFilePath)
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

    writer.StartObject();

    writer.String("typeName");
    typeName->SerializeToJson(writer);

    writer.String("subsets");
    subsets.SerializeToJson(writer);

    writer.EndObject();

    std::ofstream outputFileStream = std::ofstream(outputFilePath);

    outputFileStream << stringBuffer.GetString();
}

void SCppEntityBlueprint::SerializeToMemory(ZBinarySerializer& binarySerializer)
{
    unsigned int typeNameOffset = offsetof(SCppEntityBlueprint, typeName);
    unsigned int subsetsOffset = offsetof(SCppEntityBlueprint, subsets);

    typeName->SerializeToMemory(binarySerializer, typeNameOffset);
    subsets.SerializeToMemory(binarySerializer, subsetsOffset);
}

SCppEntityBlueprint* SCppEntityBlueprint::DeserializeFromJson(const rapidjson::Document& document)
{
    SCppEntityBlueprint* cppEntityBlueprint = new SCppEntityBlueprint();

    cppEntityBlueprint->typeName = STypeID::DeserializeFromJson(document["typeName"].GetString());
    cppEntityBlueprint->subsets = *TArray<SCppEntitySubsetInfo>::DeserializeFromJson(document["subsets"].GetArray());

    return cppEntityBlueprint;
}
