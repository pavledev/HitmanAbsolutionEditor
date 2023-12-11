#pragma once

#include "../Containers/TArray.h"
#include "SMusicTrackEntryData.h"

struct SMusicTrackData
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	TArray<SMusicTrackEntryData> m_aEntries;
	ZString m_sName;
	float m_fAttenuation;
	int m_nLowpassCutoff;
	float m_fProbability;
	ZString m_sTrackID;
};
