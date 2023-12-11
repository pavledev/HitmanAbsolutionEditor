#pragma once

#include "../ZString.h"
#include "../Containers/TArray.h"
#include "EMusicPlaylistPlayType.h"

struct SMusicPlaylistNodeData
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	int m_nSegmentIndex;
	ZString m_sName;
	int m_nParentIndex;
	TArray<int> m_aChildIndices;
	EMusicPlaylistPlayType m_ePlayType;
	int m_nWeight;
	int m_nLoopCount;
	int m_NoRepeatCount;
	int m_nSelectProbability;
	int m_nMaxLoopCount;
	int m_nRealLoopCount;
};
