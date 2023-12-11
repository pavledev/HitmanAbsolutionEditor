#pragma once

#include "rapidjson/prettywriter.h"

struct SMusicTrackVariationData
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	int m_nWeight;
	int m_nWaveIndex;
};
