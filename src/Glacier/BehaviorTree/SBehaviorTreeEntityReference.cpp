#include "Glacier/BehaviorTree/SBehaviorTreeEntityReference.h"
#include "Glacier/Serializer/ZBinarySerializer.h"

void SBehaviorTreeEntityReference::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("m_bList");
    writer.Bool(m_bList);

    writer.String("m_sName");
    m_sName.SerializeToJson(writer);

    writer.EndObject();
}

void SBehaviorTreeEntityReference::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
    unsigned int listOffset = offset + offsetof(SBehaviorTreeEntityReference, m_bList);
    unsigned int nameOffset = offset + offsetof(SBehaviorTreeEntityReference, m_sName);

    binarySerializer.WriteToMemory(&m_bList, sizeof(bool), listOffset);

    m_sName.SerializeToMemory(binarySerializer, nameOffset);
}

void SBehaviorTreeEntityReference::DeserializeFromJson(SBehaviorTreeEntityReference& behaviorTreeEntityReference, const rapidjson::Value& object)
{
    behaviorTreeEntityReference.m_bList = object["m_bList"].GetBool();
    behaviorTreeEntityReference.m_sName = *ZString::DeserializeFromJson(object["m_sName"].GetString());
}
