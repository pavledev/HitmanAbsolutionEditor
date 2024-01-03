#pragma once

#include <string>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "EWaveBankMemoryLoadRTType.h"
#include "EWaveBankRTFormat.h"

struct SWaveBankResourceData
{
	void SerializeToJson(const std::string& outputFilePath);
	static SWaveBankResourceData* DeserializeFromJson(const rapidjson::Document& document);

	int m_nTracks;
	EWaveBankMemoryLoadRTType m_eLoadType;
	EWaveBankRTFormat m_eFormat;
	int m_nIsPlaceholder;
	int m_nIsNotDucking;
	float m_fDuration;
	unsigned int m_nChannels;
	int m_nQuality;
};
