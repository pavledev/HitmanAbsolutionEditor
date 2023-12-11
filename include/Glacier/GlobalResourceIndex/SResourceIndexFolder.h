#pragma once

#include "../Containers/TArray.h"

struct SResourceIndexFolder
{
    void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
    void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
    static SResourceIndexFolder* DeserializeFromJson(const rapidjson::Value& object);

    ZString name;
    TArray<int> resourceIndices;
};
