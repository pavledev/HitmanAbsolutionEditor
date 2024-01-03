#pragma once

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

struct SScatterPackedInstance
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	unsigned char m_nData[11];
};
