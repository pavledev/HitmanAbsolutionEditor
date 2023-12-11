#pragma once

#include <vector>

#include "SShatterBoneShard.h"

struct SShatterConfig
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	std::vector<SShatterBoneShard> m_aBoneShards;
};
