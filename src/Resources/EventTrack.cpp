#include <unordered_map>

#include "Resources/EventTrack.h"

void EventTrack::Deserialize()
{
	eventSequenceHeader = static_cast<MR::EventSequenceHeader*>(buffer);

	eventSequenceHeader->Locate();
}

void EventTrack::SerializeToJson(std::string& jsonOutput)
{
	jsonOutput = eventSequenceHeader->SerializeToJson();
}
