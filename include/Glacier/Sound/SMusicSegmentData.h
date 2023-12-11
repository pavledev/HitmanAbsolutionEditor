#pragma once

#include "../ZString.h"
#include "../Containers/TArray.h"
#include "SMusicTrackData.h"
#include "SMusicTimeSignature.h"

struct SMusicSegmentData
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	ZString m_sName;
	int m_nBarCount;
	TArray<SMusicTrackData> m_aTracks;
	SMusicTimeSignature m_signature;
	int m_nChoppedSampleRate;
	int m_nChoppedSampleCount;
	int m_nLeadInSamples;
	int m_nLeadOutSamples;
};
