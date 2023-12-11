#pragma once

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

class ZBinarySerializer;

struct SBodyPartDamageMultipliers
{
    void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
    void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
    static SBodyPartDamageMultipliers* DeserializeFromJson(const rapidjson::Value& object);
    bool operator==(const SBodyPartDamageMultipliers& other) const;

    float m_fHeadDamageMultiplier;
    float m_fFaceDamageMultiplier;
    float m_fArmDamageMultiplier;
    float m_fHandDamageMultiplier;
    float m_fLegDamageMultiplier;
    float m_fTorsoDamageMultiplier;
};
