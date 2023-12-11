#pragma once

#include "rapidjson/prettywriter.h"

struct SBoneConstraintsHeader
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	unsigned int m_lNrConstraints;
};
