#pragma once

#include <vector>

#include "SKinematicBoneData.h"

struct SKinematicLinkedPrimResourceData
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	std::vector<SKinematicBoneData> m_aBonePrimitives;
};
