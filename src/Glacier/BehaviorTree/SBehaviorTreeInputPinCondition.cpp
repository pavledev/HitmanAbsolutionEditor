#include "Glacier/BehaviorTree/SBehaviorTreeInputPinCondition.h"

void SBehaviorTreeInputPinCondition::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("m_sName");
    m_sName.SerializeToJson(writer);

    writer.EndObject();
}

void SBehaviorTreeInputPinCondition::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
    unsigned int nameOffset = offset + offsetof(SBehaviorTreeInputPinCondition, m_sName);

    m_sName.SerializeToMemory(binarySerializer, nameOffset);
}

void SBehaviorTreeInputPinCondition::DeserializeFromJson(SBehaviorTreeInputPinCondition& behaviorTreeInputPinCondition, const rapidjson::Value& object)
{
    behaviorTreeInputPinCondition.m_sName = *ZString::DeserializeFromJson(object["m_sName"].GetString());
}
