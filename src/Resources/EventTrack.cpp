#include <unordered_map>

#include "Resources/EventTrack.h"

void EventTrack::Deserialize()
{
	eventSequenceHeader = static_cast<MR::EventSequenceHeader*>(buffer);

	eventSequenceHeader->Locate();
}

void EventTrack::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
}

void EventTrack::SerializeToJson(std::string& jsonOutput)
{
	jsonOutput = eventSequenceHeader->SerializeToJson();
}
