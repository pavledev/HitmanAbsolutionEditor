#pragma once

#include "SResourceIndexFolder.h"

struct SResourceIndex
{
    void SerializeToJson(const std::string& outputFilePath);
    void SerializeToMemory(ZBinarySerializer& binarySerializer);
    static SResourceIndex* DeserializeFromJson(const rapidjson::Document& document);

    TArray<SResourceIndexFolder> folders;
};
