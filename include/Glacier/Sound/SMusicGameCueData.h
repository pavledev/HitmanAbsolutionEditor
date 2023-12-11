#pragma once

#include "../ZString.h"

struct SMusicGameCueData
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	ZString m_sName;
	bool m_bIsStinger;
	int m_nTargetIndex;
};
