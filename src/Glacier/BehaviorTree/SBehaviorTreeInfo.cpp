#include "Glacier/BehaviorTree/SBehaviorTreeInfo.h"

void SBehaviorTreeInfo::SerializeToJson(const std::string& outputFilePath)
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

    writer.StartObject();

    writer.String("m_references");
    m_references.SerializeToJson(writer);

    writer.String("m_inputPinConditions");
    m_inputPinConditions.SerializeToJson(writer);

    writer.EndObject();

    std::ofstream outputFileStream = std::ofstream(outputFilePath);

    outputFileStream << stringBuffer.GetString();

    outputFileStream.close();
}

void SBehaviorTreeInfo::SerializeToMemory(ZBinarySerializer& binarySerializer)
{
    unsigned int referencesOffset = offsetof(SBehaviorTreeInfo, m_references);
    unsigned int inputPinConditionsOffset = offsetof(SBehaviorTreeInfo, m_inputPinConditions);

    m_references.SerializeToMemory(binarySerializer, referencesOffset);
    m_inputPinConditions.SerializeToMemory(binarySerializer, inputPinConditionsOffset);
}

SBehaviorTreeInfo* SBehaviorTreeInfo::DeserializeFromJson(const rapidjson::Document& document)
{
    SBehaviorTreeInfo* behaviorTreeInfo = new SBehaviorTreeInfo();

    TArray<SBehaviorTreeEntityReference>::DeserializeFromJson(behaviorTreeInfo->m_references, document["m_references"].GetArray());
    TArray<SBehaviorTreeInputPinCondition>::DeserializeFromJson(behaviorTreeInfo->m_inputPinConditions, document["m_inputPinConditions"].GetArray());

    return behaviorTreeInfo;
}
