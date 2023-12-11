#pragma once

#include <string>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "../Containers/TArray.h"
#include "SMusicGameCueData.h"
#include "SMusicPlaylistData.h"
#include "SMusicSegmentData.h"
#include "../Containers/TSparseShortArray.h"
#include "SMusicPlaylistEndBehavior.h"
#include "SMusicTransition.h"
#include "SMusicTrackExposingGroupData.h"

struct SMusicCompositionData
{
	std::string SerializeToJson();

	TArray<SMusicGameCueData> m_aGameCues;
	TArray<SMusicPlaylistData> m_aPlaylists;
	TArray<SMusicStingerData> m_aStingers;
	TArray<SMusicSegmentData> m_aSegments;
	TSparseShortArray<SMusicPlaylistEndBehavior> m_aEndBehaviors;
	TSparseShortArray<TSparseShortArray<SMusicTransition>> m_transitionMap;
	int m_nJumpCue;
	TArray<SMusicTrackExposingGroupData> m_aTrackExposingGroups;
};
