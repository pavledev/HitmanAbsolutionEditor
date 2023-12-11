#pragma once

#include "rapidjson/prettywriter.h"

struct SPoseBone
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	float m_quat[4];
	float m_pos[4];
};
