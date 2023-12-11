#include "Glacier/GlobalResourceIndex/SResourceIndex.h"

void SResourceIndex::SerializeToJson(const std::string& outputFilePath)
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

    writer.StartObject();

    writer.String("folders");
    folders.SerializeToJson(writer);

    writer.EndObject();

    std::ofstream outputFileStream = std::ofstream(outputFilePath);

    outputFileStream << stringBuffer.GetString();
}

void SResourceIndex::SerializeToMemory(ZBinarySerializer& binarySerializer)
{
    unsigned int foldersOffset = offsetof(SResourceIndex, folders);

    folders.SerializeToMemory(binarySerializer, foldersOffset);
}

SResourceIndex* SResourceIndex::DeserializeFromJson(const rapidjson::Document& document)
{
    SResourceIndex* resourceIndex = new SResourceIndex();

    resourceIndex->folders = *TArray<SResourceIndexFolder>::DeserializeFromJson(document["folders"].GetArray());

    return resourceIndex;
}
