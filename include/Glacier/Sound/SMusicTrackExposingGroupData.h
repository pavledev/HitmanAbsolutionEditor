#pragma once

#include "../ZString.h"
#include "../Containers/TArray.h"
#include "EMusicTrackExposingType.h"

struct SMusicTrackExposingGroupData
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	ZString m_sName;
	TArray<ZString> m_trackNames;
	TArray<EMusicTrackExposingType> m_trackTypes;
};
