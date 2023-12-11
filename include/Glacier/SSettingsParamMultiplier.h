#pragma once

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

#include "ESettingsParam.h"

class ZBinarySerializer;

struct SSettingsParamMultiplier
{
    void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
    void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
    static SSettingsParamMultiplier* DeserializeFromJson(const rapidjson::Value& object);

    ESettingsParam m_eSettingsParam;
    float m_fMultiplier;
};
