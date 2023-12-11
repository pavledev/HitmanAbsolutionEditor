#pragma once

#include "SQuaternion.h"

struct SQV
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	SQuaternion m_Rotation;
	float4 m_Translation;
};
