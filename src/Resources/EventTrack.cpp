#include <unordered_map>

#include "Resources/EventTrack.h"

void EventTrack::Deserialize(const std::string& filePath)
{
}

void EventTrack::Deserialize(void* buffer)
{
	eventSequenceHeader = static_cast<MR::EventSequenceHeader*>(buffer);

	eventSequenceHeader->Locate();
}

void EventTrack::SerializeToJson(std::string& jsonOutput)
{
	jsonOutput = eventSequenceHeader->SerializeToJson();
}
