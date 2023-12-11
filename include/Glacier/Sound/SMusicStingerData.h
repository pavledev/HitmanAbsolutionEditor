#pragma once

#include "EMusicPlayStartType.h"
#include "../ZCurve.h"

struct SMusicStingerData
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	int m_nSegmentIndex;
	EMusicPlayStartType m_ePlayStartType;
	bool m_bPreload;
	ZCurve m_duckingCurve;
};
