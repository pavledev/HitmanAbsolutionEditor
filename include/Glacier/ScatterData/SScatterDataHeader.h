#pragma once

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

struct SScatterDataHeader
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	unsigned int m_nType;
	unsigned int m_nMaterials;
};
