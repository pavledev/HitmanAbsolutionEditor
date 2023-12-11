#pragma once

#include "Glacier/Containers/TArray.h"

class ZSpeakerLevels
{
public:
    TArray<float>& GetSpeakerMatrix();
    void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
    void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
    static ZSpeakerLevels* DeserializeFromJson(const rapidjson::Value& object);
    bool operator==(const ZSpeakerLevels& other) const;

private:
    TArray<float> m_aSpeakerMatrix;
};
