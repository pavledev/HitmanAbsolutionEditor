#pragma once

#include "rapidjson/prettywriter.h"

#include "EMusicPlayStartType.h"
#include "EMusicTransitionType.h"
#include "EMusicTransitionFadeType.h"
#include "../ZCurve.h"

struct SMusicTransition
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	int m_nSource;
	int m_nDestination;
	EMusicPlayStartType m_eStartType;
	EMusicTransitionType m_eTransType;
	EMusicTransitionFadeType m_eFadeType;
	float m_fFadeTime;
	int m_nSegmentIndex;
	bool m_bSkipPreEntries;
	float m_fTransDelay;
	ZCurve m_customFadeInCurve;
	ZCurve m_customFadeOutCurve;
};
