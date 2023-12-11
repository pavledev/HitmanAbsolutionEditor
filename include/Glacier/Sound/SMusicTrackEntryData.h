#pragma once

#include "../Containers/TArray.h"
#include "SMusicTrackVariationData.h"

struct SMusicTrackEntryData
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	TArray<SMusicTrackVariationData> m_aVariations;
	int m_nStartBeat;
	bool m_bPlayOnce;
	int m_nSelectedVariation;
};
