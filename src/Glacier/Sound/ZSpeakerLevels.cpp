#include "Glacier/Sound/ZSpeakerLevels.h"
#include "Glacier/Serializer/ZBinarySerializer.h"

TArray<float>& ZSpeakerLevels::GetSpeakerMatrix()
{
    return m_aSpeakerMatrix;
}

void ZSpeakerLevels::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("m_aSpeakerMatrix");
    m_aSpeakerMatrix.SerializeToJson(writer);

    writer.EndObject();
}

void ZSpeakerLevels::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
    unsigned int speakerMatrixOffset = offset + offsetof(ZSpeakerLevels, m_aSpeakerMatrix);

    m_aSpeakerMatrix.SerializeToMemory(binarySerializer, speakerMatrixOffset);
}

ZSpeakerLevels* ZSpeakerLevels::DeserializeFromJson(const rapidjson::Value& object)
{
    ZSpeakerLevels* speakerLevels = new ZSpeakerLevels();

    speakerLevels->m_aSpeakerMatrix = *TArray<float>::DeserializeFromJson(object["m_aSpeakerMatrix"].GetArray());

    return speakerLevels;
}

bool ZSpeakerLevels::operator==(const ZSpeakerLevels& other) const
{
    return m_aSpeakerMatrix == other.m_aSpeakerMatrix;
}
