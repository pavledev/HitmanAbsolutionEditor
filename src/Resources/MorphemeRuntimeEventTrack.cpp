#include <unordered_map>

#include "Resources/MorphemeRuntimeEventTrack.h"

void MorphemeRuntimeEventTrack::Deserialize(const std::string& filePath)
{
}

void MorphemeRuntimeEventTrack::Deserialize(void* buffer)
{
	eventSequenceHeader = static_cast<MR::EventSequenceHeader*>(buffer);

	eventSequenceHeader->Locate();
}

void MorphemeRuntimeEventTrack::SerializeToJson(std::string& jsonOutput)
{
	jsonOutput = eventSequenceHeader->SerializeToJson();
}
