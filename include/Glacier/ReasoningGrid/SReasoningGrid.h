#pragma once

#include "SGWaypoint.h"
#include "../Containers/ZBitArray.h"
#include "SGProperties.h"

struct SReasoningGrid
{
	void SerializeToJson(const std::string& outputFilePath);
	void SerializeToMemory(ZBinarySerializer& binarySerializer);
	static SReasoningGrid* DeserializeFromJson(const rapidjson::Document& document);

	TArray<SGWaypoint> m_WaypointList;
	ZBitArray m_LowVisibilityBits;
	ZBitArray m_HighVisibilityBits;
	TArray<TArray<unsigned short>> m_LowVisibility;
	TArray<TArray<unsigned short>> m_HighVisibility;
	SGProperties m_Properties;
	unsigned int m_BitVisibilityCount;
	unsigned int m_nNodeCount;
};
