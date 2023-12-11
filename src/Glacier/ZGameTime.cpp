#include "Glacier/ZGameTime.h"
#include "Glacier/Serializer/ZBinarySerializer.h"

long long& ZGameTime::GetTicks()
{
    return m_nTicks;
}

void ZGameTime::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("m_nTicks");
    writer.Int64(m_nTicks);

    writer.EndObject();
}

void ZGameTime::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
    unsigned int ticksOffset = offset + offsetof(ZGameTime, m_nTicks);

    binarySerializer.WriteToMemory(&m_nTicks, sizeof(long long), ticksOffset);
}

ZGameTime* ZGameTime::DeserializeFromJson(const rapidjson::Value& object)
{
    ZGameTime* gameTime = new ZGameTime();

    gameTime->m_nTicks = object["m_nTicks"].GetInt64();

    return gameTime;
}

bool ZGameTime::operator==(const ZGameTime& other) const
{
    return m_nTicks == other.m_nTicks;
}
