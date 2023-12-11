#pragma once

#include "../ZString.h"
#include "../Containers/TArray.h"
#include "SMusicPlaylistNodeData.h"
#include "SMusicStingerData.h"

struct SMusicPlaylistData
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	ZString m_sName;
	TArray<SMusicPlaylistNodeData> m_aNodes;
	TArray<SMusicStingerData> m_aBursts;
	bool m_bPlayBursts;
	float m_fMinBurstDelay;
	float m_fMaxBurstDelay;
};
