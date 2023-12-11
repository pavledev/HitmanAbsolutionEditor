#include "Glacier/ReasoningGrid/SReasoningGrid.h"
#include "Glacier/Serializer/ZBinarySerializer.h"

void SReasoningGrid::SerializeToJson(const std::string& outputFilePath)
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

    writer.StartObject();

    writer.String("m_WaypointList");
    m_WaypointList.SerializeToJson(writer);

    writer.String("m_LowVisibilityBits");
    m_LowVisibilityBits.SerializeToJson(writer);

    writer.String("m_HighVisibilityBits");
    m_HighVisibilityBits.SerializeToJson(writer);

    writer.String("m_LowVisibility");
    m_LowVisibility.SerializeToJson(writer);

    writer.String("m_HighVisibility");
    m_HighVisibility.SerializeToJson(writer);

    writer.String("m_Properties");
    m_Properties.SerializeToJson(writer);

    writer.String("m_BitVisibilityCount");
    writer.Uint(m_BitVisibilityCount);

    writer.String("m_nNodeCount");
    writer.Uint(m_nNodeCount);

    writer.EndObject();

    std::ofstream outputFileStream = std::ofstream(outputFilePath);

    outputFileStream << stringBuffer.GetString();
}

void SReasoningGrid::SerializeToMemory(ZBinarySerializer& binarySerializer)
{
    unsigned int waypointListOffset = offsetof(SReasoningGrid, m_WaypointList);
    unsigned int lowVisibilityBitsOffset = offsetof(SReasoningGrid, m_LowVisibilityBits);
    unsigned int highVisibilityBitsOffset = offsetof(SReasoningGrid, m_HighVisibilityBits);
    unsigned int lowVisibilityOffset = offsetof(SReasoningGrid, m_LowVisibility);
    unsigned int highVisibilityOffset = offsetof(SReasoningGrid, m_HighVisibility);
    unsigned int propertiesOffset = offsetof(SReasoningGrid, m_Properties);
    unsigned int bitVisibilityCountOffset = offsetof(SReasoningGrid, m_BitVisibilityCount);
    unsigned int nodeCountOffset = offsetof(SReasoningGrid, m_nNodeCount);

    m_WaypointList.SerializeToMemory(binarySerializer, waypointListOffset);
    m_LowVisibilityBits.SerializeToMemory(binarySerializer, lowVisibilityBitsOffset);
    m_HighVisibilityBits.SerializeToMemory(binarySerializer, highVisibilityBitsOffset);
    m_LowVisibility.SerializeToMemory(binarySerializer, lowVisibilityOffset);
    m_HighVisibility.SerializeToMemory(binarySerializer, highVisibilityOffset);
    m_Properties.SerializeToMemory(binarySerializer, propertiesOffset);
    binarySerializer.WriteToMemory(&m_BitVisibilityCount, sizeof(unsigned int), bitVisibilityCountOffset);
    binarySerializer.WriteToMemory(&m_nNodeCount, sizeof(unsigned int), nodeCountOffset);
}

SReasoningGrid* SReasoningGrid::DeserializeFromJson(const rapidjson::Document& document)
{
    SReasoningGrid* reasoningGrid = new SReasoningGrid();

    TArray<SGWaypoint>::DeserializeFromJson(reasoningGrid->m_WaypointList, document["m_WaypointList"].GetArray());
    ZBitArray::DeserializeFromJson(reasoningGrid->m_LowVisibilityBits, document["m_LowVisibilityBits"].GetObj());
    ZBitArray::DeserializeFromJson(reasoningGrid->m_HighVisibilityBits, document["m_HighVisibilityBits"].GetObj());
    TArray<TArray<unsigned short>>::DeserializeFromJson(reasoningGrid->m_LowVisibility, document["m_LowVisibility"].GetArray());
    TArray<TArray<unsigned short>>::DeserializeFromJson(reasoningGrid->m_HighVisibility, document["m_HighVisibility"].GetArray());
    SGProperties::DeserializeFromJson(reasoningGrid->m_Properties, document["m_Properties"].GetObj());
    reasoningGrid->m_BitVisibilityCount = document["m_BitVisibilityCount"].GetUint();
    reasoningGrid->m_nNodeCount = document["m_nNodeCount"].GetUint();

    return reasoningGrid;
}
