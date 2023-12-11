#include "Glacier/SBodyPartDamageMultipliers.h"
#include "Glacier/Serializer/ZBinarySerializer.h"

void SBodyPartDamageMultipliers::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("m_fHeadDamageMultiplier");
    writer.Double(m_fHeadDamageMultiplier);

    writer.String("m_fFaceDamageMultiplier");
    writer.Double(m_fFaceDamageMultiplier);

    writer.String("m_fArmDamageMultiplier");
    writer.Double(m_fArmDamageMultiplier);

    writer.String("m_fHandDamageMultiplier");
    writer.Double(m_fHandDamageMultiplier);

    writer.String("m_fLegDamageMultiplier");
    writer.Double(m_fLegDamageMultiplier);

    writer.String("m_fTorsoDamageMultiplier");
    writer.Double(m_fTorsoDamageMultiplier);

    writer.EndObject();
}

void SBodyPartDamageMultipliers::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
    unsigned int hHeadDamageMultiplierOffset = offset + offsetof(SBodyPartDamageMultipliers, m_fHeadDamageMultiplier);
    unsigned int fFaceDamageMultiplierOffset = offset + offsetof(SBodyPartDamageMultipliers, m_fFaceDamageMultiplier);
    unsigned int armDamageMultiplierOffset = offset + offsetof(SBodyPartDamageMultipliers, m_fArmDamageMultiplier);
    unsigned int handDamageMultiplierOffset = offset + offsetof(SBodyPartDamageMultipliers, m_fHandDamageMultiplier);
    unsigned int legDamageMultiplierOffset = offset + offsetof(SBodyPartDamageMultipliers, m_fLegDamageMultiplier);
    unsigned int torsoDamageMultiplierOffset = offset + offsetof(SBodyPartDamageMultipliers, m_fTorsoDamageMultiplier);

    binarySerializer.WriteToMemory(&m_fHeadDamageMultiplier, sizeof(float), hHeadDamageMultiplierOffset);
    binarySerializer.WriteToMemory(&m_fFaceDamageMultiplier, sizeof(float), fFaceDamageMultiplierOffset);
    binarySerializer.WriteToMemory(&m_fArmDamageMultiplier, sizeof(float), armDamageMultiplierOffset);
    binarySerializer.WriteToMemory(&m_fHandDamageMultiplier, sizeof(float), handDamageMultiplierOffset);
    binarySerializer.WriteToMemory(&m_fLegDamageMultiplier, sizeof(float), legDamageMultiplierOffset);
    binarySerializer.WriteToMemory(&m_fTorsoDamageMultiplier, sizeof(float), torsoDamageMultiplierOffset);
}

SBodyPartDamageMultipliers* SBodyPartDamageMultipliers::DeserializeFromJson(const rapidjson::Value& object)
{
    SBodyPartDamageMultipliers* bodyPartDamageMultipliers = new SBodyPartDamageMultipliers();

    bodyPartDamageMultipliers->m_fHeadDamageMultiplier = object["m_fHeadDamageMultiplier"].GetFloat();
    bodyPartDamageMultipliers->m_fFaceDamageMultiplier = object["m_fFaceDamageMultiplier"].GetFloat();
    bodyPartDamageMultipliers->m_fArmDamageMultiplier = object["m_fArmDamageMultiplier"].GetFloat();
    bodyPartDamageMultipliers->m_fHandDamageMultiplier = object["m_fHandDamageMultiplier"].GetFloat();
    bodyPartDamageMultipliers->m_fLegDamageMultiplier = object["m_fLegDamageMultiplier"].GetFloat();
    bodyPartDamageMultipliers->m_fTorsoDamageMultiplier = object["m_fTorsoDamageMultiplier"].GetFloat();

    return bodyPartDamageMultipliers;
}

bool SBodyPartDamageMultipliers::operator==(const SBodyPartDamageMultipliers& other) const
{
    return m_fHeadDamageMultiplier == other.m_fHeadDamageMultiplier &&
        m_fFaceDamageMultiplier == other.m_fFaceDamageMultiplier &&
        m_fArmDamageMultiplier == other.m_fArmDamageMultiplier &&
        m_fHandDamageMultiplier == other.m_fHandDamageMultiplier &&
        m_fLegDamageMultiplier == other.m_fLegDamageMultiplier &&
        m_fTorsoDamageMultiplier == other.m_fTorsoDamageMultiplier;
}
