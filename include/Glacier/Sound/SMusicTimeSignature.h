#pragma once

#include "rapidjson/prettywriter.h"

struct SMusicTimeSignature
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	int m_nBeatsPerMinute;
	int m_nBeatsPerBar;
};
