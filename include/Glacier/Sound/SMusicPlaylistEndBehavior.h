#pragma once

#include "rapidjson/prettywriter.h"

struct SMusicPlaylistEndBehavior
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	bool m_bGotoPrevious;
	int m_nAttenuationOffset;
};
